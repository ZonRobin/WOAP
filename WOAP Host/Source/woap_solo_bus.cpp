#include "woap_solo_bus.h"
#include "woap_parameter_bool.h"
#include "woap_track.h"
#include "woap_audio_output_proxy.h"
#include "woap_api_messages_solo_bus.h"
#include "woap_user_permission.h"
#include "woap_api_listener_bridges_audio_core_listener_bridge.h"

using namespace WOAP;
using namespace WOAP::API;
using namespace WOAP::API::ListenerBridges;
using namespace WOAP::API::Messages;

SoloBus::SoloBus(AudioCore* core)
{
	setName("Solo output bus");
	setNumOutputChannels(NumChannels::Stereo);

	audioOutputBuffer.clearWithHistory();

	core->addListener(this);
	initializeAPIConsumer();
}

SoloBus::~SoloBus()
{
	terminateAPIConsumer();
}

const Bootstrapper::Initializer SoloBus::bootstrapInitializer([](Bootstrapper& bootstrapper) {
	bootstrapper.reportPermissions({
		new UserPermission(Permission::ListenAudioData, "Listen solo bus audio data")
	});
});

void SoloBus::gatherData(int numSamples)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	audioOutputBuffer.setSize(getNumOutputChannels(), numSamples);
	const int numInputs = soloedTracks.size();
	
	if (numInputs == 0)
	{
		audioOutputBuffer.clear();
	}
	else if (numInputs == 1)
	{
		audioOutputBuffer.copyFrom(0, soloedTracks[0]->getSoloOutputBuffer(), 0, numSamples);
	}
	else
	{
		audioOutputBuffer.clear();
		for (const Track* sourceTrack : soloedTracks)
		{
			audioOutputBuffer.addFrom(0, sourceTrack->getSoloOutputBuffer(), 0, numSamples);
		}
	}

	signalLevels.setUnchecked(0, (float)audioOutputBuffer.getMagnitude(0, 0, numSamples));
	signalLevels.setUnchecked(1, (float)audioOutputBuffer.getMagnitude(1, 0, numSamples));

	if (audioDataListeners.size() != 0)
	{
		const ScopedLock sl(audioDataListeners.getListeners().getLock());
		audioDataListeners.call(&AudioDataEmitter::Listener::audioProcessed, this, audioOutputBuffer);
	}
}

void SoloBus::trackAdded(Track* track, int)
{
	const ScopedLock sl(getProcessLock());

	SoloListener* listener = new SoloListener(this, track);
	track->getSoloParameter()->addListener(listener);
	soloListeners.add(listener);

	if (*track->getSoloParameter())
		soloedTracks.add(track);
}

void SoloBus::trackRemoved(Track* track)
{
	const ScopedLock sl(getProcessLock());

	soloedTracks.removeFirstMatchingValue(track);
	for (SoloListener* listener : soloListeners)
	{
		if (listener->track == track) 
		{
			track->getSoloParameter()->removeListener(listener);
			soloListeners.removeObject(listener);
			return;
		}
	}
}

bool SoloBus::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	return nullptr != dynamic_cast<const AudioOutputProxy*>(receiver);
}

int SoloBus::getLatencyDiffForNode(const MixerNode*) const
{
	return 0;
}

int SoloBus::getLatencySamplesInTree(bool) const
{
	return 0;
}

ValueTree SoloBus::getState() const
{
	ValueTree state("SOLOBUS");

	StateHelper::setIdProperty(state, this);

	return state;
}

void SoloBus::setState(const ValueTree& state)
{
	if (!state.hasType("SOLOBUS"))
		return;

	setId(StateHelper::getIdProperty(state));
}

Array<AddressWithTypeId> SoloBus::getMessageTypesToListenTo() const
{
	return Array<AddressWithTypeId>({
		AddressWithTypeId(SoloBusAddAudioDataListener::getAddress(), MessageType::AddAudioDataListener),
		AddressWithTypeId(SoloBusRemoveAudioDataListener::getAddress(), MessageType::RemoveAudioDataListener)
	});
}

void SoloBus::messageReceived(OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId)
{
	switch (messageTypeId)
	{
	case MessageType::AddAudioDataListener:
	{
		SoloBusAddAudioDataListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ListenAudioData))
			return;

		AudioCoreAudioDataListenerBridge::addToList<AudioDataEmitter::Listener>(
			audioDataListeners, 
			new AudioCoreAudioDataListenerBridge(*this, client, msg.getMinNumSamples())
		);
		break;
	}
	case MessageType::RemoveAudioDataListener:
	{
		SoloBusRemoveAudioDataListener msg(message);
		if (!testValidityAndPermissions(client, msg, Permission::ListenAudioData))
			return;

		AudioCoreAudioDataListenerBridge::removeFromList(audioDataListeners, *this, client);
		break;
	}
	default:
		handleUnknownMessage("SoloBus", client, message, messageTypeId);
		break;
	}
}

void SoloBus::SoloListener::parameterChanged(Parameter* param)
{
	const ScopedLock sl(soloBus->getProcessLock());

	if (static_cast<ParameterBool*>(param)->get())
		soloBus->soloedTracks.add(track);
	else
		soloBus->soloedTracks.removeFirstMatchingValue(track);
}

