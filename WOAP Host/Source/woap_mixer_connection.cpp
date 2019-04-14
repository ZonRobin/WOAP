#include "woap_mixer_connection.h"
#include "woap_mixer_source_node.h"
#include "woap_mixer_receiver_node.h"

using namespace WOAP;


MixerConnection::MixerConnection(MixerSourceNode* from, MixerReceiverNode* to) : inputSource(from), outputReceiver(to)
{
	inputSource->addOutputConnection(this);
	outputReceiver->addInputConnection(this);
}

MixerConnection::~MixerConnection()
{
	inputSource->removeOutputConnection(this);
	outputReceiver->removeInputConnection(this);
}

MixerSourceNode* MixerConnection::getInputSource() const
{
	return inputSource;
}

MixerReceiverNode* MixerConnection::getOutputReceiver() const
{
	return outputReceiver;
}