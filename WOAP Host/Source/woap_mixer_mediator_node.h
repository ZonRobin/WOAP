#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_source_node.h"
#include "woap_mixer_receiver_node.h"

namespace WOAP
{
	class MixerMediatorNode : public MixerSourceNode, public MixerReceiverNode
	{
	public:
		MixerMediatorNode();
		virtual ~MixerMediatorNode();

		virtual void processBlock(const AudioBufferDouble& inputBuffer) = 0;

		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;
		virtual int getLatencyDiffForNode(const MixerNode* node) const override;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerMediatorNode)
	};
}
