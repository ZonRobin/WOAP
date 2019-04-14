#include "woap_mixer_node.h"

using namespace WOAP;

MixerNode::MixerNode()
	: processLock(new CriticalSection(), true)
{
	
}

MixerNode::MixerNode(const Uuid idToUse)
	: Identifiable(idToUse), processLock(new CriticalSection(), true)
{
}

MixerNode::~MixerNode()
{
	listeners.call(&Listener::aboutToDelete, this);
}

void MixerNode::setName(const String& newName)
{
	if (name == newName)
		return;

	name = newName;
	nameChanged();
	listeners.call(&Listener::nameChanged, this);
}

void MixerNode::setColour(Colour newColour)
{
	if (colour == newColour)
		return;

	colour = newColour;
	colourChanged();
	listeners.call(&Listener::colourChanged, this);
}

int MixerNode::getLatencySamples() const
{
	return 0;
}