#include "woap_audio_input_proxy.h"
#include "woap_mixer_connection.h"
#include "woap_mixer_receiver_node.h"
#include "woap_audio_core.h"
#include "woap_track.h"

using namespace WOAP;

AudioInputProxy::AudioInputProxy(const String& name, int channelIndex)
{
	setName(name);
	assignChannels(channelIndex);
}

AudioInputProxy::AudioInputProxy(const String& name, int leftChannelIndex, int rightChannelIndex)
{
	setName(name);
	assignChannels(leftChannelIndex, rightChannelIndex);
}

AudioInputProxy::AudioInputProxy(const String& name, const Array<int>& channelIndexes)
{
	setName(name);
	assignChannels(channelIndexes);
}

AudioInputProxy::AudioInputProxy(const Uuid& idToUse, const String& name, const Array<int>& channelIndexes)
	: MixerNode(idToUse)
{
	setName(name);
	assignChannels(channelIndexes);
}

AudioInputProxy::~AudioInputProxy()
{

}

void AudioInputProxy::assignChannels(int channelIndex)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (channelIndex < 0) 
	{
		jassertfalse;
		return;
	}

	const ScopedLock sl(getProcessLock());

	channelIndexes.clearQuick();
	channelIndexes.add(channelIndex);
	setNumOutputChannels(NumChannels::Mono);
	audioOutputBuffer.clearWithHistory();
}

void AudioInputProxy::assignChannels(int leftChannelIndex, int rightChannelIndex)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (leftChannelIndex < 0 || rightChannelIndex < 0) 
	{
		jassertfalse;
		return;
	}

	const ScopedLock sl(getProcessLock());

	channelIndexes.clearQuick();
	channelIndexes.add(leftChannelIndex);
	channelIndexes.add(rightChannelIndex);
	setNumOutputChannels(NumChannels::Stereo);
	audioOutputBuffer.clearWithHistory();
}

void AudioInputProxy::assignChannels(const Array<int>& newChannelIndexes)
{
	ASSERT_ONLY_MESSAGE_THREAD;

	if (newChannelIndexes.size() > 2) 
	{
		jassertfalse;
		return;
	}

	for (int index : newChannelIndexes)
	{
		if (index < 0) 
		{
			jassertfalse;
			return;
		}
	}

	const ScopedLock sl(getProcessLock());

	channelIndexes.clearQuick();
	channelIndexes.addArray(newChannelIndexes);
	setNumOutputChannels(channelIndexes.size());
	audioOutputBuffer.clearWithHistory();
}

void AudioInputProxy::gatherData(const float** inputChannelData, int numInputChannels, int numSamples)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	audioOutputBuffer.setSize(getNumOutputChannels(), numSamples, globalAudioLatencySamples + 64);
	audioOutputBuffer.shiftSamplesToHistory();

	switch (getNumOutputChannels())
	{
	case NumChannels::Mono:
	{
		const int channelIndex = channelIndexes[0];

		if (channelIndex >= numInputChannels)
		{
			audioOutputBuffer.clear();
			jassertfalse;
			return;
		}

		audioOutputBuffer.copyFrom(0, 0, inputChannelData[channelIndex], numSamples);
		break;
	}
	case NumChannels::Stereo:
	{
		const int leftChannelIndex = channelIndexes[0];
		const int rightChannelIndex = channelIndexes[1];

		if (leftChannelIndex >= numInputChannels || rightChannelIndex >= numInputChannels)
		{
			audioOutputBuffer.clear();
			jassertfalse;
			return;
		}

		audioOutputBuffer.copyFrom(0, 0, inputChannelData[leftChannelIndex], numSamples);
		audioOutputBuffer.copyFrom(1, 0, inputChannelData[rightChannelIndex], numSamples);
		break;
	}
	default:
		audioOutputBuffer.clear();
		break;
	}
}

void AudioInputProxy::setGlobalAudioLatencySamples(int newNumSamples)
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	if (newNumSamples < 0) 
	{
		jassertfalse;
		newNumSamples = 0;
	}

	globalAudioLatencySamples = newNumSamples;
}

bool AudioInputProxy::canConnectOutputReceiver(const MixerReceiverNode* receiver) const
{
	return nullptr != dynamic_cast<const AudioTrack*>(receiver);
}

int AudioInputProxy::getLatencyDiffForNode(const MixerNode* node) const
{
	return globalAudioLatencySamples - node->getLatencySamplesInTree();
}

ValueTree AudioInputProxy::getState() const
{
	ValueTree state("AUDIOINPUTPROXY");

	StateHelper::setIdProperty(state, this);
	state.setProperty("name", getName(), nullptr);

	ValueTree channelsState("CHANNELS");
	for (int channel : getAssignedChannels())
	{
		channelsState.addChild(ValueTree("CHANNEL").setProperty("index", channel, nullptr), -1, nullptr);
	}
	state.addChild(channelsState, -1, nullptr);

	return state;
}

void AudioInputProxy::setState(const ValueTree& state)
{
	if (!state.hasType("AUDIOINPUTPROXY"))
		return;

	setName(StateHelper::getStringProperty(state, "name"));

	Array<int> channelsToAssign;
	for (ValueTree channelState : state.getChildWithName("CHANNELS"))
	{
		if (!channelState.hasType("CHANNEL"))
			continue;

		const int chan = StateHelper::getIntProperty(channelState, "index", -1);
		if (chan >= 0)
			channelsToAssign.addIfNotAlreadyThere(chan);
	}
	assignChannels(channelsToAssign);
}

AudioInputProxy* AudioInputProxy::fromState(const ValueTree& state)
{
	if (!state.hasType("AUDIOINPUTPROXY"))
		return nullptr;

	const Uuid stateId = StateHelper::getIdProperty(state);
	if (stateId.isNull())
		return nullptr;

	AudioInputProxy* proxy = new AudioInputProxy(stateId, String());
	proxy->setState(state);
	return proxy;
}
