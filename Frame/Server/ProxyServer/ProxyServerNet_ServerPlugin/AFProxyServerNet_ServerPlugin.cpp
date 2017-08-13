// -------------------------------------------------------------------------
//    @FileName         :    NFGameServerNet_ServerPlugin.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-07-14 08:51
//    @Module           :    NFGameServerNet_ServerPlugin
//
// -------------------------------------------------------------------------


#include "AFCProxyServerNet_ServerModule.h"
#include "AFProxyServerNet_ServerPlugin.h"

//
//
#ifdef NF_DYNAMIC_PLUGIN

ARK_EXPORT void DllStartPlugin(AFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, NFProxyServerNet_ServerPlugin)

};

ARK_EXPORT void DllStopPlugin(AFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFProxyServerNet_ServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFProxyServerNet_ServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFProxyServerNet_ServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(NFProxyServerNet_ServerPlugin)
}

void NFProxyServerNet_ServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, AFIProxyServerNet_ServerModule, AFCProxyServerNet_ServerModule)

}

void NFProxyServerNet_ServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, AFIProxyServerNet_ServerModule, AFCProxyServerNet_ServerModule)
}