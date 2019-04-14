#include "woap_mixer_mediator_node.h"

using namespace WOAP;

MixerMediatorNode::MixerMediatorNode() 
{

}

MixerMediatorNode::~MixerMediatorNode()
{

}

int MixerMediatorNode::getLatencySamplesInTree(bool includeOwnLatency) const
{
	return MixerSourceNode::getLatencySamplesInTree(includeOwnLatency);
}

int MixerMediatorNode::getLatencyDiffForNode(const MixerNode* node) const
{
	const int latency = getLatencySamplesInTree(false) - node->getLatencySamplesInTree();
	jassert(latency >= 0);

	return jmax(0, latency);
}