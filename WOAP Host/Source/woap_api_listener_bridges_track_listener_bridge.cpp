#include "woap_api_listener_bridges_track_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_parameter_bool.h"
#include "woap_parameter_float.h"
#include "woap_api_messages_track.h"
#include "woap_audio_processor_player_double.h"
#include "woap_track_send_node.h"

using namespace WOAP;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::OSC;

TrackInfoListenerBridge::TrackInfoListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackInfoListenerBridge::~TrackInfoListenerBridge()
{
	trackedInstance.removeListener(this);
}

void TrackInfoListenerBridge::nameChanged(MixerNode*)
{
	client.sendOSCMessage(Messages::TrackNameChanged(&trackedInstance, trackedInstance.getName()));
}

void TrackInfoListenerBridge::colourChanged(MixerNode*)
{
	client.sendOSCMessage(Messages::TrackColourChanged(&trackedInstance, trackedInstance.getColour()));
}

void TrackInfoListenerBridge::aboutToDelete(MixerNode*)
{
	client.removeListenerBridge(this);
}


TrackGainListenerBridge::TrackGainListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackGainListenerBridge::~TrackGainListenerBridge()
{
	trackedInstance.getGainParameter()->removeListener(this);
}

void TrackGainListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::TrackGainChanged(&trackedInstance, trackedInstance.getGainParameter()->get()));
}

void TrackGainListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}



TrackPanListenerBridge::TrackPanListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackPanListenerBridge::~TrackPanListenerBridge()
{
	trackedInstance.getPanParameter()->removeListener(this);
}

void TrackPanListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::TrackPanChanged(&trackedInstance, trackedInstance.getPanParameter()->get()));
}

void TrackPanListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


TrackMuteListenerBridge::TrackMuteListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackMuteListenerBridge::~TrackMuteListenerBridge()
{
	trackedInstance.getMuteParameter()->removeListener(this);
}

void TrackMuteListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::TrackMuteChanged(&trackedInstance, *trackedInstance.getMuteParameter()));
}

void TrackMuteListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


TrackSoloListenerBridge::TrackSoloListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{
	
}

TrackSoloListenerBridge::~TrackSoloListenerBridge()
{
	trackedInstance.getSoloParameter()->removeListener(this);
}

void TrackSoloListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::TrackSoloChanged(&trackedInstance, trackedInstance.getSoloParameter()->get()));
}

void TrackSoloListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


TrackRecordArmListenerBridge::TrackRecordArmListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{
	
}

TrackRecordArmListenerBridge::~TrackRecordArmListenerBridge()
{
	trackedInstance.getRecordArmParameter()->removeListener(this);
}

void TrackRecordArmListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::TrackRecordArmChanged(&trackedInstance, trackedInstance.getRecordArmParameter()->get()));
}

void TrackRecordArmListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}


TrackPluginsListenerBridge::TrackPluginsListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackPluginsListenerBridge::~TrackPluginsListenerBridge()
{
	trackedInstance.removePluginsListener(this);
}

void TrackPluginsListenerBridge::pluginAdded(AudioProcessorPlayerDouble* plugin, int position)
{
	client.sendOSCMessage(Messages::TrackPluginAdded(&trackedInstance, plugin, position));
}

void TrackPluginsListenerBridge::pluginRemoved(AudioProcessorPlayerDouble* plugin)
{
	client.sendOSCMessage(Messages::TrackPluginRemoved(&trackedInstance, plugin));
}

void TrackPluginsListenerBridge::pluginMoved(AudioProcessorPlayerDouble* plugin, int oldPosition, int newPosition)
{
	client.sendOSCMessage(Messages::TrackPluginMoved(&trackedInstance, plugin, oldPosition, newPosition));
}

void TrackPluginsListenerBridge::aboutToDelete(Track*)
{
	client.removeListenerBridge(this);
}



TrackSendsListenerBridge::TrackSendsListenerBridge(Track& track, RemoteClient& client)
	: OSCBridgeBase(track, client)
{

}

TrackSendsListenerBridge::~TrackSendsListenerBridge()
{
	trackedInstance.removeSendsListener(this);
}

void TrackSendsListenerBridge::sendAdded(TrackSendNode* node, int position)
{
	client.sendOSCMessage(Messages::TrackSendAdded(&trackedInstance, node, position));
}

void TrackSendsListenerBridge::sendRemoved(TrackSendNode* node)
{
	client.sendOSCMessage(Messages::TrackSendRemoved(&trackedInstance, node));
}

void TrackSendsListenerBridge::sendMoved(TrackSendNode* node, int oldPosition, int newPosition)
{
	client.sendOSCMessage(Messages::TrackSendMoved(&trackedInstance, node, oldPosition, newPosition));
}

void TrackSendsListenerBridge::aboutToDelete(Track*)
{
	client.removeListenerBridge(this);
}