#include "woap_api_listener_bridges_plugin_manager_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_plugin_manager.h"

using namespace WOAP;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::OSC;

PluginManagerListenerBridge::PluginManagerListenerBridge(PluginManager& pluginManager, RemoteClient& client)
	: OSCBridgeBase(pluginManager, client)
{
}

PluginManagerListenerBridge::~PluginManagerListenerBridge()
{
	trackedInstance.removeListener(this);
}

void PluginManagerListenerBridge::pluginTypesScanStarted()
{
	client.sendOSCMessage(Messages::PluginManagerPluginTypesScanStarted());
}

void PluginManagerListenerBridge::pluginTypesScanned(int newNumPlugins)
{
	client.sendOSCMessage(Messages::PluginManagerPluginTypesScanned(newNumPlugins));
}

void PluginManagerListenerBridge::pluginTypeAdded(const PluginDescription& desc)
{
	client.sendOSCMessage(Messages::PluginManagerPluginTypeAdded(desc));
}
