#include "woap_audio_output_proxy.h"
#include "woap_mixer_connection.h"
#include "woap_mixer_source_node.h"
#include "woap_track.h"
#include "woap_solo_bus.h"

using namespace WOAP;

AudioOutputProxy::AudioOutputProxy(const String& name, int channelIndex)
{
	setName(name);
	assignChannels(channelIndex);
}

AudioOutputProxy::AudioOutputProxy(const String& name, int leftChannelIndex, int rightChannelIndex)
{
	setName(name);
	assignChannels(leftChannelIndex, rightChannelIndex);
}

AudioOutputProxy::AudioOutputProxy(const String& name, const Array<int>& channelIndexes)
{
	setName(name);
	assignChannels(channelIndexes);
}

AudioOutputProxy::AudioOutputProxy(const Uuid& id, const String& name, const Array<int>& channelIndexes)
	: MixerNode(id)
{
	setName(name);
	assignChannels(channelIndexes);
}

AudioOutputProxy::~AudioOutputProxy()
{

}

void AudioOutputProxy::assignChannels(int channelIndex)
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
	setNumInputChannels(NumChannels::Mono);
}

void AudioOutputProxy::assignChannels(int leftChannelIndex, int rightChannelIndex)
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
	setNumInputChannels(NumChannels::Stereo);
}

void AudioOutputProxy::assignChannels(const Array<int>& newChannelIndexes)
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
	setNumInputChannels(channelIndexes.size());
}

void AudioOutputProxy::streamData(float** outputChannelData, int numOutputChannels, int numSamples) const
{
	ASSERT_ONLY_AUDIO_PROCESS_THREADS;

	AudioBufferFloat outputBuffer;
	outputBuffer.setDataToReferTo(outputChannelData, numOutputChannels, numSamples);
	
	switch (getNumInputChannels())
	{
	case NumChannels::Mono:
	{
		const int channelIndex = channelIndexes[0];
		if (channelIndex >= numOutputChannels)
		{
			jassertfalse;
			return;
		}

		for (const MixerConnection* connection : inputConnections)
		{
			const AudioBufferDouble& sourceBuffer = connection->getInputSource()->getAudioOutputBuffer();

			switch (sourceBuffer.getNumChannels())
			{
			case NumChannels::Mono:
				outputBuffer.addFrom(channelIndex, 0, sourceBuffer.getReadPointer(0), numSamples);
				break;
			case NumChannels::Stereo:
				outputBuffer.addFrom(channelIndex, 0, sourceBuffer.getReadPointer(0), numSamples, 0.5f);
				outputBuffer.addFrom(channelIndex, 0, sourceBuffer.getReadPointer(1), numSamples, 0.5f);
				break;
			default:
				break;
			}
		}
		break;
	}
	case NumChannels::Stereo:
	{
		const int leftChannelIndex = channelIndexes[0];
		const int rightChannelIndex = channelIndexes[1];

		if (leftChannelIndex >= numInputChannels || rightChannelIndex >= numInputChannels)
		{
			jassertfalse;
			return;
		}
		
		for (const MixerConnection* connection : inputConnections)
		{
			const AudioBufferDouble& sourceBuffer = connection->getInputSource()->getAudioOutputBuffer();

			switch (sourceBuffer.getNumChannels())
			{
			case NumChannels::Mono:
				outputBuffer.addFrom(leftChannelIndex, 0, sourceBuffer.getReadPointer(0), numSamples);
				outputBuffer.addFrom(rightChannelIndex, 0, sourceBuffer.getReadPointer(0), numSamples);
				break;
			case NumChannels::Stereo:
				outputBuffer.addFrom(leftChannelIndex, 0, sourceBuffer.getReadPointer(0), numSamples);
				outputBuffer.addFrom(rightChannelIndex, 0, sourceBuffer.getReadPointer(1), numSamples);
				break;
			default:
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

bool AudioOutputProxy::canConnectInputSource(const MixerSourceNode* source) const
{
	return nullptr != dynamic_cast<const OutputBusTrack*>(source) 
		|| nullptr != dynamic_cast<const SoloBus*>(source);
}

int AudioOutputProxy::getLatencyDiffForNode(const MixerNode*) const
{
	return 0;
}

int AudioOutputProxy::getLatencySamplesInTree(bool) const
{
	return 0;
}

ValueTree AudioOutputProxy::getState() const
{
	ValueTree state("AUDIOOUTPUTPROXY");

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

void AudioOutputProxy::setState(const ValueTree& state)
{
	if (!state.hasType("AUDIOOUTPUTPROXY"))
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

AudioOutputProxy* AudioOutputProxy::fromState(const ValueTree& state)
{
	if (!state.hasType("AUDIOOUTPUTPROXY"))
		return nullptr;

	const Uuid stateId = StateHelper::getIdProperty(state);
	if (stateId.isNull())
		return nullptr;

	AudioOutputProxy* proxy = new AudioOutputProxy(stateId, String());
	proxy->setState(state);
	return proxy;
}
