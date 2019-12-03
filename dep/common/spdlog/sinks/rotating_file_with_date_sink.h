// Copyright(c) 2019 ARK
// Based on rotating_file_sink
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#pragma once

#ifndef SPDLOG_H
#include "spdlog/spdlog.h"
#endif

#include "spdlog/details/file_helper.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/base_sink.h"

#include <cerrno>
#include <chrono>
#include <ctime>
#include <mutex>
#include <string>
#include <tuple>

namespace spdlog {
namespace sinks {

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#elif _LINUX
#include <stdarg.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a), 0755)
#endif
//
// Rotating file sink based on size
//
template<typename Mutex>
class rotating_file_with_date_sink final : public base_sink<Mutex>
{
public:
    rotating_file_with_date_sink(filename_t base_filename, std::size_t max_size, std::size_t max_files)
        : base_filename_(std::move(base_filename))
        , max_size_(max_size)
        , max_files_(max_files)
    {
        file_helper_.open(calc_filename(base_filename_, 0));
        current_size_ = file_helper_.size(); // expensive. called only once
    }

    //
    // return file pre-dir and name and its extension:
    //
    // "mylog.txt" => ("", "mylog", ".txt")
    // "mylog" => ("", "mylog", "")
    // "mylog." => ("", "mylog.", "")
    // "/dir1/dir2/mylog.txt" => ("/dir1/dir2/", "mylog", ".txt")
    //
    // the starting dot in filenames is ignored (hidden files):
    //
    // ".mylog" => ("", ".mylog". "")
    // "my_folder/.mylog" => ("my_folder/", ".mylog", "")
    // "my_folder/.mylog.txt" => ("my_folder/", ".mylog", ".txt")
    static std::tuple<filename_t, filename_t, filename_t> split_by_dir_and_extenstion(const spdlog::filename_t& fname)
    {
        auto ext_index = fname.rfind('.');

        // no valid extension found - return whole path and empty string as extension
        if (ext_index == filename_t::npos || ext_index == 0 || ext_index == fname.size() - 1)
        {
            return std::make_tuple(spdlog::filename_t(), fname, spdlog::filename_t());
        }

        // treat cases like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
        auto folder_index = fname.rfind(details::os::folder_sep);
        if (folder_index != fname.npos && folder_index >= ext_index - 1)
        {
            return std::make_tuple(fname.substr(0, folder_index), fname.substr(folder_index + 1), spdlog::filename_t());
        }

        // finally - return a valid base and extension tuple
        return std::make_tuple(fname.substr(0, folder_index),
            fname.substr(folder_index + 1, ext_index - folder_index - 1), fname.substr(ext_index));
    }

    // calc filename according to index and file extension if exists.
    // e.g. calc_filename("logs/mylog.txt, 3) => "logs/mylog.3.txt".
    static filename_t calc_filename(const filename_t& filename, std::size_t index)
    {
        std::tm tm = spdlog::details::os::localtime();
        filename_t pre_dir, basename, ext;
        std::tie(pre_dir, basename, ext) = split_by_dir_and_extenstion(filename);
        auto dir_path =
            fmt::format(SPDLOG_FILENAME_T("{}{}{:04d}{:02d}{:02d}{}"), pre_dir, spdlog::details::os::folder_sep,
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, spdlog::details::os::folder_sep);

        // CHECK if directory is already existed
#if _WIN32
        char tmp_dir_path[MAX_PATH] = {0};
#else
        char tmp_dir_path[PATH_MAX] = {0};
#endif
        for (size_t i = 0; i < dir_path.length(); ++i)
        {
            tmp_dir_path[i] = dir_path[i];
            if (tmp_dir_path[i] == spdlog::details::os::folder_sep)
            {
                if (ACCESS(tmp_dir_path, 0) == -1)
                {
                    MKDIR(tmp_dir_path);
                }
            }
        }

        typename std::conditional<std::is_same<filename_t::value_type, char>::value, fmt::memory_buffer,
            fmt::wmemory_buffer>::type w;
        if (index != 0u)
        {
            fmt::format_to(w, SPDLOG_FILENAME_T("{}{}.{}{}"), dir_path, basename, index, ext);
        }
        else
        {
            fmt::format_to(w, SPDLOG_FILENAME_T("{}{}{}"), dir_path, basename, ext);
        }

        return fmt::to_string(w);
    }

protected:
    void sink_it_(const details::log_msg& msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        current_size_ += formatted.size();
        if (current_size_ > max_size_)
        {
            rotate_();
            current_size_ = formatted.size();
        }
        file_helper_.write(formatted);
    }

    void flush_() override
    {
        file_helper_.flush();
    }

private:
    // Rotate files:
    // log.txt -> log.1.txt
    // log.1.txt -> log.2.txt
    // log.2.txt -> log.3.txt
    // log.3.txt -> delete
    void rotate_()
    {
        using details::os::filename_to_str;
        file_helper_.close();
        for (auto i = max_files_; i > 0; --i)
        {
            filename_t src = calc_filename(base_filename_, i - 1);
            if (!details::file_helper::file_exists(src))
            {
                continue;
            }
            filename_t target = calc_filename(base_filename_, i);

            if (!rename_file(src, target))
            {
                // if failed try again after a small delay.
                // this is a workaround to a windows issue, where very high rotation
                // rates can cause the rename to fail with permission denied (because of antivirus?).
                details::os::sleep_for_millis(100);
                if (!rename_file(src, target))
                {
                    file_helper_.reopen(true); // truncate the log file anyway to prevent it to grow beyond its limit!
                    current_size_ = 0;
                    throw spdlog_ex("rotating_file_sink: failed renaming " + filename_to_str(src) + " to " +
                                        filename_to_str(target),
                        errno);
                }
            }
        }
        file_helper_.reopen(true);
    }

    // delete the target if exists, and rename the src file  to target
    // return true on success, false otherwise.
    bool rename_file(const filename_t& src_filename, const filename_t& target_filename)
    {
        // try to delete the target file in case it already exists.
        (void)details::os::remove(target_filename);
        return details::os::rename(src_filename, target_filename) == 0;
    }

    filename_t base_filename_;
    std::size_t max_size_;
    std::size_t max_files_;
    std::size_t current_size_;
    details::file_helper file_helper_;
};

using rotating_file_with_date_sink_mt = rotating_file_with_date_sink<std::mutex>;
using rotating_file_with_date_sink_st = rotating_file_with_date_sink<details::null_mutex>;

} // namespace sinks

//
// factory functions
//

template<typename Factory = default_factory>
inline std::shared_ptr<logger> rotating_logger_mt(
    const std::string& logger_name, const filename_t& filename, size_t max_file_size, size_t max_files)
{
    return Factory::template create<sinks::rotating_file_with_date_sink_mt>(
        logger_name, filename, max_file_size, max_files);
}

template<typename Factory = default_factory>
inline std::shared_ptr<logger> rotating_logger_st(
    const std::string& logger_name, const filename_t& filename, size_t max_file_size, size_t max_files)
{
    return Factory::template create<sinks::rotating_file_with_date_sink_st>(
        logger_name, filename, max_file_size, max_files);
}
} // namespace spdlog
