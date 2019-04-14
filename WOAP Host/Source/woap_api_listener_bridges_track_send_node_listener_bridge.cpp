#include "woap_api_listener_bridges_track_send_node_listener_bridge.h"
#include "woap_api_osc_remote_client.h"
#include "woap_parameter_float.h"
#include "woap_api_messages_track_send_node.h"

using namespace WOAP;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::OSC;

SendInfoListenerBridge::SendInfoListenerBridge(TrackSendNode& send, RemoteClient& client)
	: OSCBridgeBase(send, client)
{

}

SendInfoListenerBridge::~SendInfoListenerBridge()
{
	trackedInstance.removeListener(this);
}

void SendInfoListenerBridge::nameChanged(MixerNode*)
{
	client.sendOSCMessage(Messages::SendNameChanged(&trackedInstance, trackedInstance.getName()));
}

void SendInfoListenerBridge::colourChanged(MixerNode*)
{
	client.sendOSCMessage(Messages::SendColourChanged(&trackedInstance, trackedInstance.getColour()));
}

void SendInfoListenerBridge::aboutToDelete(MixerNode*)
{
	client.removeListenerBridge(this);
}


SendGainListenerBridge::SendGainListenerBridge(TrackSendNode& send, RemoteClient& client)
	: OSCBridgeBase(send, client)
{

}

SendGainListenerBridge::~SendGainListenerBridge()
{
	trackedInstance.getGainParameter()->removeListener(this);
}

void SendGainListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::SendGainChanged(&trackedInstance, trackedInstance.getGainParameter()->get()));
}

void SendGainListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}



SendPanListenerBridge::SendPanListenerBridge(TrackSendNode& send, RemoteClient& client)
	: OSCBridgeBase(send, client)
{

}

SendPanListenerBridge::~SendPanListenerBridge()
{
	trackedInstance.getPanParameter()->removeListener(this);
}

void SendPanListenerBridge::parameterChanged(Parameter*)
{
	client.sendOSCMessage(Messages::SendPanChanged(&trackedInstance, trackedInstance.getPanParameter()->get()));
}

void SendPanListenerBridge::parameterWillBeDeleted(Parameter*)
{
	client.removeListenerBridge(this);
}

SendAudioSourcePositionListenerBridge::SendAudioSourcePositionListenerBridge(TrackSendNode& send, RemoteClient& client)
	: OSCBridgeBase(send, client)
{
}

SendAudioSourcePositionListenerBridge::~SendAudioSourcePositionListenerBridge()
{
	trackedInstance.removeAudioSourcePositionListener(this);
}

void SendAudioSourcePositionListenerBridge::sendAudioSourcePositionChanged(TrackSendNode*)
{
	client.sendOSCMessage(Messages::SendAudioSourcePositionChanged(&trackedInstance, trackedInstance.getAudioSourcePosition()));
}

void SendAudioSourcePositionListenerBridge::aboutToDelete(TrackSendNode*)
{
	client.removeListenerBridge(this);
}
