#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_source_node.h"
#include "woap_stateful.h"

namespace WOAP
{
	class AudioInputProxy : public MixerSourceNode, public Stateful
	{
	public:
		AudioInputProxy(const String& name, int channelIndex);
		AudioInputProxy(const String& name, int leftChannelIndex, int rightChannelIndex);
		AudioInputProxy(const String& name = "Input", const Array<int>& channelIndexes = Array<int>());
		AudioInputProxy(const Uuid& idToUse, const String& name = "Input", const Array<int>& channelIndexes = Array<int>());
		virtual ~AudioInputProxy();

		void assignChannels(int channelIndex);
		void assignChannels(int leftChannelIndex, int rightChannelIndex);
		void assignChannels(const Array<int>& channelIndexes);

		bool hasChannelAssigned(int channelIndex) { return channelIndexes.contains(channelIndex); }
		const Array<int>& getAssignedChannels() const { return channelIndexes; }

		void gatherData(const float** inputChannelData, int numInputChannels, int numSamples);
		
		void setGlobalAudioLatencySamples(int newNumSamples);

		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const;
		virtual int getLatencyDiffForNode(const MixerNode* node) const override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static AudioInputProxy* fromState(const ValueTree& state);

	protected:
		Array<int> channelIndexes;

		int globalAudioLatencySamples = 0;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioInputProxy)
	};
}
