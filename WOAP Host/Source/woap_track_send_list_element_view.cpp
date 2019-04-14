#include "woap_track_send_list_element_view.h"
#include "woap_track.h"
#include "woap_main_view.h"
#include "woap_track_send_node_detail_view.h"
#include "woap_drag_handle.h"
#include "woap_look_and_feel.h"
#include "woap_simple_call_out_box.h"
#include "woap_name_label.h"
#include "woap_track_send_node.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackSendListElementView::TrackSendListElementView(TrackSendNode* send, Track* sendOwner) 
	: trackSendNode(send), track(sendOwner)
{
	setMouseCursor(MouseCursor::PointingHandCursor);

	dragHandle = new DragHandle();
	dragHandle->addMouseListener(this, true);
	addAndMakeVisible(dragHandle);

	sendNameLabel = new NameLabel();
	sendNameLabel->setText(send->getName(), NotificationType::dontSendNotification);
	sendNameLabel->addMouseListener(this, true);
	sendNameLabel->setBackgroundMargin(1, 1);
	addAndMakeVisible(sendNameLabel);

	track->addListener(this);
	trackSendNode->addListener(this);

	updateColours();
}

TrackSendListElementView::~TrackSendListElementView()
{
	SimpleCallOutBox::close(sendDetailCallOut);
	track->removeListener(this);
	trackSendNode->removeListener(this);
}

void TrackSendListElementView::selfDelete() const
{
	MessageManager::callAsync([this]() {
		track->removeSend(trackSendNode);
	});
}

void TrackSendListElementView::resized()
{
	sendNameLabel->setBounds(getLocalBounds().withWidth(getWidth() - getHeight() - 5));
	dragHandle->setBounds(getWidth() - getHeight() - 4, 4, getHeight() - 6, getHeight() - 6);
}

void TrackSendListElementView::paint(Graphics& g)
{
	g.setColour(track->getColour().brighter(0.3f));
	g.fillRoundedRectangle(getLocalBounds().reduced(1, 1).toFloat(), 3.0f);
}

void TrackSendListElementView::updateColours()
{
	sendNameLabel->setBackgroundColour(
		track->getColour().brighter(0.3f),
		track->getColour().getPerceivedBrightness() > 0.5f
	);

	repaint();
}

void TrackSendListElementView::mouseUp(const MouseEvent& ev)
{
	if (ev.eventComponent == sendNameLabel)
	{
		if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
		{
			TrackSendNodeDetailView* view = new TrackSendNodeDetailView(trackSendNode, track);
			view->setSize(200, 380);
			sendDetailCallOut = SimpleCallOutBox::open(view, getScreenBounds());
		}
	}
}

void TrackSendListElementView::mouseDown(const MouseEvent& ev)
{
	if (ev.eventComponent == dragHandle)
	{
		MainView::getInstance()->startDragging(var(), this);
	}
}

void TrackSendListElementView::nameChanged(MixerNode*)
{
	sendNameLabel->setText(trackSendNode->getName(), NotificationType::dontSendNotification);
}
