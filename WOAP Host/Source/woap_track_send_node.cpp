#include "woap_track_send_node.h"
#include "woap_parameter_float.h"
#include "woap_track.h"
#include "woap_api_osc_remote_client.h"
#include "woap_api_messages_track_send_node.h"
#include "woap_api_listener_bridges_track_send_node_listener_bridge.h"
#include "woap_look_and_feel.h"
#include "woap_audio_core.h"
#include "woap_parameter_bool.h"

using namespace WOAP;
using namespace API::Messages;
using namespace API::ListenerBridges;

TrackSendNode::TrackSendNode(Track& ownerTrack, int audioSourcePos)
	: track(ownerTrack), audioSourcePosition(audioSourcePos)
{
	setNumOutputChannels(NumChannels::Stereo);

	panParameter = new ParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f);
	gainParameter = new ParameterFloat("gain", "Gain", -145.0f, 10.0f, 0.0f);

	channelGains.add(new LinearSmoothedValue<double>());
	channelGains.add(new LinearSmoothedValue<double>());

	AudioCore::getInstance()->addAudioPlaybackInfoListener(this);

	initializeAPIConsumer();
}

TrackSendNode::TrackSendNode(const Uuid& idToUse, Track& ownerTrack, int audioSourcePos)
	: MixerNode(idToUse), track(ownerTrack), audioSourcePosition(audioSourcePos)
{
	setNumOutputChannels(NumChannels::Stereo);

	panParameter = new ParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f);
	gainParameter = new ParameterFloat("gain", "Gain", -145.0f, 10.0f, 0.0f);

	channelGains.add(new LinearSmoothedValue<double>());
	channelGains.add(new LinearSmoothedValue<double>());

	AudioCore::getInstance()->addAudioPlaybackInfoListener(this);

	initializeAPIConsumer();
}

TrackSendNode::~TrackSendNode()
{
	terminateAPIConsumer();
	AudioCore::getInstance()->removeAudioPlaybackInfoListener(this);

	audioSourcePositionListeners.call(&AudioSourcePositionListener::aboutToDelete, this);
}

void TrackSendNode::playbackAboutToStart(double newSampleRate, int)
{
	channelGains[0]->reset(newSampleRate, GainSmoothingTimeSecs);
	channelGains[1]->reset(newSampleRate, GainSmoothingTimeSecs);

	audioOutputBuffer.clearWithHistory();
}

void TrackSendNode::processBlock(const AudioBufferDouble& inputBuffer)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	const int numSamples = inputBuffer.getNumCurrentSamples();

	if (treeLatencySamples < 0)
		treeLatencySamples = getLatencySamplesInTree();
	
	audioOutputBuffer.setSize(getNumOutputChannels(), numSamples, treeLatencySamples);
	audioOutputBuffer.shiftSamplesToHistory();

	double leftChannelGain = 0;
	double rightChannelGain = 0;

	if (!*track.getMuteParameter())
	{
		leftChannelGain = Decibels::decibelsToGain(gainParameter->get(), -145.0f);
		rightChannelGain = Decibels::decibelsToGain(gainParameter->get(), -145.0f);

		if (panParameter->get() > 0)
			leftChannelGain = leftChannelGain - panParameter->get() * leftChannelGain;
		else
			rightChannelGain = rightChannelGain + panParameter->get() * rightChannelGain;
	}
	
	channelGains[0]->setValue(leftChannelGain);
	channelGains[1]->setValue(rightChannelGain);

	channelGains[0]->applyGain(audioOutputBuffer.getWritePointer(0), inputBuffer.getReadPointer(0), numSamples);
	channelGains[1]->applyGain(audioOutputBuffer.getWritePointer(1), inputBuffer.getReadPointer(1), numSamples);

	ASSERT_BROKEN_BUFFER_DATA(audioOutputBuffer);
}

bool TrackSendNode::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	if (this == receiver)
		return false;

	if (&track == receiver)
		return false;

	if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(receiver))
		if (source->hasOutputReceiverAssigned(&track))
			return false;

	return nullptr != dynamic_cast<const FxTrack*>(receiver)
		|| nullptr != dynamic_cast<const MixBusTrack*>(receiver)
		|| nullptr != dynamic_cast<const OutputBusTrack*>(receiver);
}

bool TrackSendNode::canConnectInputSource(const MixerSourceNode*) const
{
	return false;
}

bool TrackSendNode::hasInputSourceAssigned(const MixerSourceNode* sourceNode, bool recursive) const
{
	if (track.hasInputSourceAssigned(sourceNode, recursive))
		return true;

	return false;
}

int TrackSendNode::getLatencySamplesInTree(bool includeOwnLatency) const
{
	if (includeOwnLatency)
		return track.getLatencySamplesInTree();
	else
		return track.getLatencySamplesInTree(false) + track.getLatencySamples() - track.getLatencySamplesForAudioSource(audioSourcePosition);
}

void TrackSendNode::setAudioSourcePosition(int newPosition)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (audioSourcePosition == newPosition)
		return;

	const ScopedLock sl(getProcessLock());

	audioSourcePosition = newPosition;
	audioSourcePositionListeners.call(&AudioSourcePositionListener::sendAudioSourcePositionChanged, this);
}

Array<API::AddressWithTypeId> TrackSendNode::getMessageTypesToListenTo() const
{
	const String idStr = getId().toDashedString();

	return Array<API::AddressWithTypeId>({
		API::AddressWithTypeId(SendGetDetail::getAddress(idStr), MessageType::GetDetail),
		API::AddressWithTypeId(SendSetGain::getAddress(idStr), MessageType::SetGain),
		API::AddressWithTypeId(SendSetPan::getAddress(idStr), MessageType::SetPan),
		API::AddressWithTypeId(SendSetAudioSourcePosition::getAddress(idStr), MessageType::SetAudioSourcePosition),
		API::AddressWithTypeId(SendSetName::getAddress(idStr), MessageType::SetName),
		API::AddressWithTypeId(SendAddAudioSourcePositionListener::getAddress(idStr), MessageType::AddAudioSourcePositionListener),
		API::AddressWithTypeId(SendRemoveAudioSourcePositionListener::getAddress(idStr), MessageType::RemoveAudioSourcePositionListener),
		API::AddressWithTypeId(SendAddInfoListener::getAddress(idStr), MessageType::AddInfoListener),
		API::AddressWithTypeId(SendRemoveInfoListener::getAddress(idStr), MessageType::RemoveInfoListener),
		API::AddressWithTypeId(SendAddControlsListener::getAddress(idStr), MessageType::AddControlsListener),
		API::AddressWithTypeId(SendRemoveControlsListener::getAddress(idStr), MessageType::RemoveControlsListener)
	});
}

void TrackSendNode::messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::GetDetail:
	{
		SendGetDetail msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		client.sendOSCMessage(
			SendDetail(
				msg,
				this,
				*gainParameter,
				*panParameter,
				getName(),
				getColour(),
				audioSourcePosition
		));
		break;
	}
	case MessageType::SetGain:
	{
		SendSetGain msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ModifyTrack))
			return;

		*gainParameter = msg.getNewValue();
		break;
	}
	case MessageType::SetPan:
	{
		SendSetPan msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ModifyTrack))
			return;

		*panParameter = msg.getNewValue();
		break;
	}
	case MessageType::SetAudioSourcePosition:
	{
		SendSetAudioSourcePosition msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ModifyTrack))
			return;

		setAudioSourcePosition(msg.getNewPosition());
		break;
	}
	case MessageType::SetName:
	{
		SendSetName msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ModifyTrack))
			return;

		setName(msg.getNewName());
		break;
	}
	case MessageType::AddAudioSourcePositionListener:
	{
		SendAddAudioSourcePositionListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendAudioSourcePositionListenerBridge::addToList<AudioSourcePositionListener>(
			audioSourcePositionListeners, new SendAudioSourcePositionListenerBridge(*this, client)
		);
		break;
	}
	case MessageType::RemoveAudioSourcePositionListener:
	{
		SendRemoveAudioSourcePositionListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendAudioSourcePositionListenerBridge::removeFromList(audioSourcePositionListeners, *this, client);
		break;
	}
	case MessageType::AddInfoListener:
	{
		SendAddInfoListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendInfoListenerBridge::addToList<Listener>(listeners, new SendInfoListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveInfoListener:
	{
		SendRemoveInfoListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendInfoListenerBridge::removeFromList(listeners, *this, client);
		break;
	}
	case MessageType::AddControlsListener:
	{
		SendAddControlsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendGainListenerBridge::addToList<WOAP::Parameter::Listener>(getGainParameter()->getListeners(), new SendGainListenerBridge(*this, client));
		SendPanListenerBridge::addToList<WOAP::Parameter::Listener>(getPanParameter()->getListeners(), new SendPanListenerBridge(*this, client));
		break;
	}
	case MessageType::RemoveControlsListener:
	{
		SendRemoveControlsListener msg(message, this);
		if (!testValidityAndPermissions(client, msg, Track::Permission::ViewTrack))
			return;

		SendGainListenerBridge::removeFromList(gainParameter->getListeners(), *this, client);
		SendPanListenerBridge::removeFromList(panParameter->getListeners(), *this, client);
		break;
	}
	default:
		handleUnknownMessage(getName(), client, message, messageTypeId);
		break;
	}
}

ValueTree TrackSendNode::getState() const
{
	ValueTree state("SEND");

	StateHelper::setIdProperty(state, this);
	state.setProperty("name", getName(), nullptr);
	state.setProperty("audioSourcePosition", getAudioSourcePosition(), nullptr);
	state.setProperty("gain", var(*gainParameter), nullptr);
	state.setProperty("pan", var(*panParameter), nullptr);

	return state;
}

void TrackSendNode::setState(const ValueTree& state)
{
	if (!state.hasType("SEND"))
		return;

	setName(StateHelper::getStringProperty(state, "name"));
	setAudioSourcePosition(StateHelper::getIntProperty(state, "audioSourcePosition", -1));
	*gainParameter = StateHelper::getFloatProperty(state, "gain");
	*panParameter = StateHelper::getFloatProperty(state, "pan");
}

TrackSendNode* TrackSendNode::fromState(Track& track, const ValueTree& state)
{
	if (!state.hasType("SEND"))
		return nullptr;

	const Uuid stateId = StateHelper::getIdProperty(state);
	if (stateId.isNull())
		return nullptr;

	TrackSendNode* send = new TrackSendNode(stateId, track);
	send->setState(state);

	return send;
}

Colour TrackSendNode::getDefaultColour()
{
	return CustomColours::green;
}

