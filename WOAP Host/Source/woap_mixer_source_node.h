#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_node.h"
#include "woap_audio_buffer_extended.h"

namespace WOAP
{
	class MixerConnection;
	class MixerReceiverNode;

	class MixerSourceNode : virtual public MixerNode
	{
	public:
		MixerSourceNode();
		virtual ~MixerSourceNode();

		inline const AudioBufferDouble& getAudioOutputBuffer() const noexcept { return audioOutputBuffer; }

		bool assignOutputReceiver(MixerReceiverNode* const receiver);
		void removeOutputReceiver(MixerReceiverNode* const receiver);

		inline const Array<const MixerConnection*>& getOutputConnections() const noexcept { return outputConnections; }
		Array<MixerReceiverNode*> getOutputReceivers() const noexcept;
		bool hasAnyOutputReceiverAssigned() const noexcept;

		virtual bool canConnectOutputReceiver(const MixerReceiverNode* receiver) const = 0;
		virtual bool hasOutputReceiverAssigned(const MixerReceiverNode* receiver, bool recursive = true) const;

		void addOutputConnection(MixerConnection* const connection);
		void removeOutputConnection(MixerConnection* const connection);

		Array<MixerReceiverNode*> getAvailableOutputReceivers() const;

		void setNumOutputChannels(int numOutputChannels);
		inline int getNumOutputChannels() const noexcept { return audioOutputBuffer.getNumChannels(); }

		virtual int getLatencySamplesInTree(bool includeOwnLatency = true) const override;

	protected:
		AudioBufferDouble audioOutputBuffer;
		Array<const MixerConnection*> outputConnections;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerSourceNode)
	};
}
