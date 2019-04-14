#include "woap_api_listener_bridges_plugin_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_plugin.h"
#include "woap_parameter_bool.h"

using namespace WOAP;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::OSC;

PluginListenerBridge::PluginListenerBridge(AudioProcessorPlayerDouble& plugin, RemoteClient& client)
	: OSCBridgeBase(plugin, client)
{

}

PluginListenerBridge::~PluginListenerBridge()
{
	trackedInstance.removeListener(this);
}

void PluginListenerBridge::nameChanged(AudioProcessorPlayerDouble* plugin)
{
	client.sendOSCMessage(Messages::PluginNameChanged(plugin, trackedInstance.getName()));
}

void PluginListenerBridge::aboutToDelete(AudioProcessorPlayerDouble*)
{
	client.removeListenerBridge(this);
}



PluginBypassListenerBridge::PluginBypassListenerBridge(AudioProcessorPlayerDouble& plugin, RemoteClient& client)
	: OSCBridgeBase(plugin, client)
{

}

PluginBypassListenerBridge::~PluginBypassListenerBridge()
{
	trackedInstance.getBypassParameter()->removeListener(this);
}

void PluginBypassListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::PluginBypassChanged(&trackedInstance, trackedInstance.getBypassParameter()->get()));
}

void PluginBypassListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


PluginActiveListenerBridge::PluginActiveListenerBridge(AudioProcessorPlayerDouble& plugin, RemoteClient& client)
	: OSCBridgeBase(plugin, client)
{

}

PluginActiveListenerBridge::~PluginActiveListenerBridge()
{
	trackedInstance.getActiveStateParameter()->removeListener(this);
}

void PluginActiveListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::PluginActiveChanged(&trackedInstance, trackedInstance.getActiveStateParameter()->get()));
}

void PluginActiveListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


PluginParameterListenerBridge::PluginParameterListenerBridge(AudioProcessorPlayerDouble& plugin, RemoteClient& client)
	: OSCBridgeBase(plugin, client)
{

}

PluginParameterListenerBridge::~PluginParameterListenerBridge()
{
	trackedInstance.removeParameterListener(this);
}

void PluginParameterListenerBridge::audioProcessorParameterChanged(AudioProcessorPlayerDouble*, int parameterIndex, float newValue)
{
	client.sendOSCMessage(Messages::PluginParameterValueChanged(&trackedInstance, parameterIndex, newValue, trackedInstance.getCurrentProcessor()->getParameterText(parameterIndex, 1024)));
}

void PluginParameterListenerBridge::audioProcessorParameterChangeGestureBegin(AudioProcessorPlayerDouble*, int parameterIndex)
{
	ignoreUnused(parameterIndex);
}

void PluginParameterListenerBridge::audioProcessorParameterChangeGestureEnd(AudioProcessorPlayerDouble*, int parameterIndex)
{
	ignoreUnused(parameterIndex);
}

void PluginParameterListenerBridge::aboutToDelete(AudioProcessorPlayerDouble*)
{
	client.removeListenerBridge(this);
}