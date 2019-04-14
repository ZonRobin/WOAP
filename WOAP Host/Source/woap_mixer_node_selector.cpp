#include "woap_mixer_node_selector.h"
#include "woap_look_and_feel.h"
#include "woap_track_send_node.h"
#include "woap_mixer_source_node.h"
#include "woap_mixer_receiver_node.h"
#include "woap_track_send_node.h"
#include "woap_track.h"

using namespace WOAP;
using namespace WOAP::GUI;


MixerInputNodeSelector::MixerInputNodeSelector(int width, MixerReceiverNode* r)
	: FlexList<MixerIONodeSelectorNode>(FlexList<MixerIONodeSelectorNode>::Direction::Vertical)
{
	setScrollOnDragEnabled(true);
	setSize(width, 0);
	
	setReceiverNode(r);
	AudioCore::getInstance()->addListener(this);
}

MixerInputNodeSelector::~MixerInputNodeSelector()
{
	AudioCore::getInstance()->removeListener(this);
}

void MixerInputNodeSelector::setReceiverNode(MixerReceiverNode* receiver)
{
	receiverNode = receiver;
	updateList();
}

void MixerInputNodeSelector::paint(Graphics& g)
{
	g.fillAll(findColour(ResizableWindow::backgroundColourId));
}

void MixerInputNodeSelector::updateList()
{
	const Point<int> viewPos = getViewPosition();
	removeAllItems();
	removeEmptyPlaceholder(false);

	if (receiverNode == nullptr) {
		addEmptyPlaceholder(getWidth(), 35);
		return;
	}

	if (receiverNode->getAvailableInputSources().isEmpty()) {
		addEmptyPlaceholder(getWidth(), 35, false);
	}

	for (MixerSourceNode* availableSource : receiverNode->getAvailableInputSources())
		addItem(new MixerIONodeSelectorNode(true, availableSource, receiverNode), 35, false);
		
	setSize(getWidth(), jmax(35, getIdealSize()));
	refresh(viewPos);
}

void MixerInputNodeSelector::connectionsChanged()
{
	updateList();
}



/* OUTPUT ---------------------------------------------------------------------- */


MixerOutputNodeSelector::MixerOutputNodeSelector(int width, MixerSourceNode* s)
	: FlexList<MixerIONodeSelectorNode>(FlexList<MixerIONodeSelectorNode>::Direction::Vertical)
{
	setScrollOnDragEnabled(true);
	setSize(width, 0);

	setSourceNode(s);
	AudioCore::getInstance()->addListener(this);
}

MixerOutputNodeSelector::~MixerOutputNodeSelector()
{
	AudioCore::getInstance()->removeListener(this);
}

void MixerOutputNodeSelector::setSourceNode(MixerSourceNode* source)
{
	sourceNode = source;
	updateList();
}

void MixerOutputNodeSelector::paint(Graphics& g)
{
	g.fillAll(findColour(ResizableWindow::backgroundColourId));
}

void MixerOutputNodeSelector::updateList()
{
	const Point<int> viewPos = getViewPosition();
	removeAllItems();
	removeEmptyPlaceholder(false);

	if (sourceNode == nullptr) {
		addEmptyPlaceholder(getWidth(), 35);
		return;
	}

	if (sourceNode->getAvailableOutputReceivers().isEmpty()) {
		addEmptyPlaceholder(getWidth(), 35, false);
	}

	for (MixerReceiverNode* availableOutput : sourceNode->getAvailableOutputReceivers())
		addItem(new MixerIONodeSelectorNode(false, sourceNode, availableOutput), 35, false);

	setSize(getWidth(), jmax(35, getIdealSize()));
	refresh(viewPos);
}

void MixerOutputNodeSelector::connectionsChanged()
{
	updateList();
}

/* NODE ---------------------------------------------------------------------- */


MixerIONodeSelectorNode::MixerIONodeSelectorNode(bool sourceNode, MixerSourceNode* s, MixerReceiverNode* r)
	: isSourceNode(sourceNode), source(s), receiver(r)
{
	setMouseCursor(MouseCursor::PointingHandCursor);

	if (isSourceNode)
		selected = source->hasOutputReceiverAssigned(receiver, false);
	else
		selected = receiver->hasInputSourceAssigned(source, false);

	MixerNode* nodeToListen = isSourceNode ? (MixerNode*)source : receiver;
	nodeToListen->addListener(this);
	nameChanged(nodeToListen);
}

MixerIONodeSelectorNode::~MixerIONodeSelectorNode()
{
	MixerNode* nodeToListen = isSourceNode ? (MixerNode*)source : receiver;
	nodeToListen->removeListener(this);
}

void MixerIONodeSelectorNode::paint(Graphics& g)
{
	if (selected)
		g.setColour(CustomColours::green);
	else
		g.setColour(Colour(0xFFAAAAAA).interpolatedWith(isSourceNode ? source->getColour() : receiver->getColour(), 0.10f));

	g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(1.0f, 1.0f), 3.0f);

	g.setColour(CustomColours::darkTextColour);
	g.drawFittedText(nameToShow, getLocalBounds().reduced(5, 0), Justification::centred, 2);
}

void MixerIONodeSelectorNode::mouseUp(const MouseEvent& ev)
{
	if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
	{
		MessageManager::callAsync([this]() {
			if (selected)
				source->removeOutputReceiver(receiver);
			else
				source->assignOutputReceiver(receiver);
		});
	}
}

void MixerIONodeSelectorNode::nameChanged(MixerNode* node)
{
	if (TrackSendNode* send = dynamic_cast<TrackSendNode*>(node))
		nameToShow = send->getOwnerTrack().getName() + " - " + send->getName();
	else
		nameToShow = node->getName();
}
