#include "SDK/Interface/AFPlatform.h"

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "Dbghelp.lib" )
#endif

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN

#ifdef NF_DYNAMIC_PLUGIN
#pragma comment( lib, "Core_d.lib" )

#else
#pragma comment( lib, "Core_d.lib" )
#endif

//
//#pragma comment( lib, "libglog_d.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core_d.a" )
//#pragma comment( lib, "libglog_d.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#ifdef NF_DYNAMIC_PLUGIN
#pragma comment( lib, "Core.lib" )
#else
#pragma comment( lib, "Core.lib" )
#endif

//
//#pragma comment( lib, "libglog.lib" )
#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "Core.a" )
//#pragma comment( lib, "libglog.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif
