#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	class MixerConnection;
	class MixerSourceNode;

	class MixerReceiverNode : virtual public MixerNode
	{
	public:
		MixerReceiverNode();
		virtual ~MixerReceiverNode();

		inline const Array<const MixerConnection*>& getInputConnections() const noexcept { return inputConnections; };
		Array<MixerSourceNode*> getInputSources() const noexcept;
		bool hasAnyInputSourceAssigned() const noexcept;

		virtual bool canConnectInputSource(const MixerSourceNode* source) const = 0;
		virtual bool hasInputSourceAssigned(const MixerSourceNode* sourceNode, bool recursive = true) const;

		bool assignInputSource(MixerSourceNode* const source);
		void removeInputSource(MixerSourceNode* const source);

		void addInputConnection(MixerConnection* const connection);
		void removeInputConnection(MixerConnection* const connection);

		Array<MixerSourceNode*> getAvailableInputSources() const;

		inline int getNumInputChannels() const noexcept { return numInputChannels; }
		void setNumInputChannels(int numChannels);

	protected:
		int numInputChannels;
		Array<const MixerConnection*> inputConnections;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerReceiverNode)
	};
}
