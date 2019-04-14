#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class MixerSourceNode;
	class MixerReceiverNode;

	class MixerConnection
	{
	public:
		MixerConnection(MixerSourceNode* from, MixerReceiverNode* to);
		virtual ~MixerConnection();

		bool isConnecting(MixerSourceNode* source, MixerReceiverNode* receiver) const noexcept { 
			return inputSource == source && outputReceiver == receiver; 
		}

		MixerSourceNode* getInputSource() const;
		MixerReceiverNode* getOutputReceiver() const;

	protected:
		MixerSourceNode* const inputSource;
		MixerReceiverNode* const outputReceiver;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerConnection)
	};
}
