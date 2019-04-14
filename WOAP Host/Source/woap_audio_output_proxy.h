#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_receiver_node.h"
#include "woap_stateful.h"

namespace WOAP
{
	class AudioOutputProxy : public MixerReceiverNode, public Stateful
	{
	public:
		AudioOutputProxy(const String& name, int channelIndex);
		AudioOutputProxy(const String& name, int leftChannelIndex, int rightChannelIndex);
		AudioOutputProxy(const String& name = "Output", const Array<int>& channelIndexes = Array<int>());
		AudioOutputProxy(const Uuid& id, const String& name = "Output", const Array<int>& channelIndexes = Array<int>());
		virtual ~AudioOutputProxy();

		void assignChannels(int channelIndex);
		void assignChannels(int leftChannelIndex, int rightChannelIndex);
		void assignChannels(const Array<int>& channelIndexes);

		bool hasChannelAssigned(int channelIndex) const { return channelIndexes.contains(channelIndex); }
		const Array<int>& getAssignedChannels() const { return channelIndexes; }

		void streamData(float** outputChannelData, int numOutputChannels, int numSamples) const;
		virtual bool canConnectInputSource(const MixerSourceNode* source) const override;

		virtual int getLatencyDiffForNode(const MixerNode* node) const override;
		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static AudioOutputProxy* fromState(const ValueTree& state);

	protected:
		Array<int> channelIndexes;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioOutputProxy)
	};
}

