#include "woap_mixer_source_node.h"
#include "woap_mixer_connection.h"
#include "woap_audio_core.h"
#include "woap_mixer_receiver_node.h"

using namespace WOAP;

MixerSourceNode::MixerSourceNode()
{

}

MixerSourceNode::~MixerSourceNode()
{
	
}

bool MixerSourceNode::assignOutputReceiver(MixerReceiverNode* const receiver)
{
	return AudioCore::getInstance()->connect(this, receiver);
}

void MixerSourceNode::removeOutputReceiver(MixerReceiverNode* const receiver)
{
	AudioCore::getInstance()->removeConnection(this, receiver);
}

Array<MixerReceiverNode*> MixerSourceNode::getOutputReceivers() const noexcept
{
	Array<MixerReceiverNode*> receivers;
	for (const MixerConnection* connection : outputConnections)
	{
		receivers.add(connection->getOutputReceiver());
	}
	return receivers;
}

bool MixerSourceNode::hasAnyOutputReceiverAssigned() const noexcept
{
	return outputConnections.size() != 0;
}

bool MixerSourceNode::hasOutputReceiverAssigned(const MixerReceiverNode* receiver, bool recursive) const
{
	if (!recursive)
	{
		for (const MixerConnection* connection : outputConnections)
			if (connection->getOutputReceiver() == receiver)
				return true;

		return false;
	}
	
	for (const MixerConnection* connection : outputConnections)
	{
		if (connection->getOutputReceiver() == receiver)
			return true;

		if (const MixerSourceNode* source = dynamic_cast<const MixerSourceNode*>(connection->getOutputReceiver()))
			if (source->hasOutputReceiverAssigned(receiver, recursive))
				return true;
	}

	return false;
}

void MixerSourceNode::addOutputConnection(MixerConnection* const connection)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	outputConnections.add(connection);
}

void MixerSourceNode::removeOutputConnection(MixerConnection* const connection)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	outputConnections.removeFirstMatchingValue(connection);
}

Array<MixerReceiverNode*> MixerSourceNode::getAvailableOutputReceivers() const
{
	ASSERT_ONLY_MESSAGE_THREAD;
	return AudioCore::getInstance()->getAvailableOutputReceivers(this);
}

void MixerSourceNode::setNumOutputChannels(int numChannels)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	jassert(numChannels >= 0 && numChannels <= 2);

	const ScopedLock sl(getProcessLock());
	audioOutputBuffer.setSize(numChannels, audioOutputBuffer.getNumCurrentSamples());
}

int MixerSourceNode::getLatencySamplesInTree(bool includeOwnLatency) const
{
	int maxLatency = 0;

	for (const MixerConnection* connection : outputConnections)
	{
		maxLatency = jmax(maxLatency, connection->getOutputReceiver()->getLatencySamplesInTree());
	}

	if (includeOwnLatency)
		return maxLatency + getLatencySamples();
	else
		return maxLatency;
}