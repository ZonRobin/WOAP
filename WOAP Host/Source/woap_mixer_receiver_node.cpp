#include "woap_mixer_receiver_node.h"
#include "woap_mixer_connection.h"
#include "woap_mixer_source_node.h"
#include "woap_audio_core.h"

using namespace WOAP;

MixerReceiverNode::MixerReceiverNode() 
	: numInputChannels(0)
{

}

MixerReceiverNode::~MixerReceiverNode()
{
	
}

bool MixerReceiverNode::hasAnyInputSourceAssigned() const noexcept
{
	return inputConnections.size() != 0;
}

bool MixerReceiverNode::hasInputSourceAssigned(const MixerSourceNode* sourceNode, bool recursive) const
{
	if (!recursive)
	{
		for (const MixerConnection* connection : inputConnections)
			if (connection->getInputSource() == sourceNode)
				return true;

		return false;
	}

	for (const MixerConnection* connection : inputConnections)
	{
		if (connection->getInputSource() == sourceNode)
			return true;

		if (MixerReceiverNode* receiver = dynamic_cast<MixerReceiverNode*>(connection->getInputSource()))
			if (receiver->hasInputSourceAssigned(sourceNode, recursive))
				return true;
	}

	return false;
}

Array<MixerSourceNode*> MixerReceiverNode::getInputSources() const noexcept
{
	Array<MixerSourceNode*> sources;
	for (const MixerConnection* connection : inputConnections)
	{
		sources.add(connection->getInputSource());
	}
	return sources;
}

bool MixerReceiverNode::assignInputSource(MixerSourceNode* const source)
{
	return AudioCore::getInstance()->connect(source, this);
}

void MixerReceiverNode::removeInputSource(MixerSourceNode* const source)
{
	AudioCore::getInstance()->removeConnection(source, this);
}

void MixerReceiverNode::addInputConnection(MixerConnection* const connection)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	inputConnections.add(connection);
}

void MixerReceiverNode::removeInputConnection(MixerConnection* const connection)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	inputConnections.removeFirstMatchingValue(connection);
}

Array<MixerSourceNode*> MixerReceiverNode::getAvailableInputSources() const
{
	ASSERT_ONLY_MESSAGE_THREAD;
	return AudioCore::getInstance()->getAvailableInputSources(this);
}

void MixerReceiverNode::setNumInputChannels(int numChannels)
{
	ASSERT_ONLY_MESSAGE_THREAD;
	jassert(numChannels >= 0 && numChannels <= 2);

	const ScopedLock sl(getProcessLock());
	numInputChannels = numChannels;
}