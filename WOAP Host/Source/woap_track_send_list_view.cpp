#include "woap_track_send_list_view.h"
#include "woap_track_send_node.h"
#include "woap_button_list.h"
#include "woap_simple_call_out_box.h"
#include "woap_audio_processor_player_double.h"
#include "woap_icon_button.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackSendListView::TrackSendListView(Track* tr)
	: track(nullptr)
{
	sendList = new FlexList<TrackSendListElementView>(FlexList<TrackSendListElementView>::Direction::Vertical);
	sendList->setScrollOnDragEnabled(true);
	sendList->setSwapGetsureEnabled(true);
	sendList->addListener(this);
	sendList->setAutoScrollSpeed(1);
	sendList->setAutoScrollDistance(40);
	addAndMakeVisible(sendList);

	addSendButton = new IconButton(FontAwesome_Plus);
	addSendButton->addListener(this);
	addSendButton->setShowBackground(false);
	addAndMakeVisible(addSendButton);

	headingLabel = new Label("Heading", "Sends");
	addAndMakeVisible(headingLabel);

	setTrack(tr);
}

TrackSendListView::~TrackSendListView()
{
	setTrack(nullptr);
}

void TrackSendListView::paint(Graphics& g)
{
	Path head;
	head.addRoundedRectangle(0, 0, (float)getWidth(), 35.0f, 3.0f, 3.0f, true, true, false, false);
	g.setColour(Colour(60, 60, 60));
	g.fillPath(head);

	Path fill;
	fill.addRoundedRectangle(0, 35.0f, (float)getWidth(), (float)getHeight() - 35.0f, 3.0f, 3.0f, false, false, true, true);
	g.setColour(Colour(215, 215, 215));
	g.fillPath(fill);
}

void TrackSendListView::resized()
{
	sendList->setBounds(getLocalBounds().withTrimmedTop(35));

	headingLabel->setBounds(10, 2, 100, 30);
	addSendButton->setBounds(getWidth() - 45, 5, 40, 26);
}

void TrackSendListView::setTrack(Track* tr)
{
	if (track != nullptr)
	{
		track->removeSendsListener(this);
		sendList->removeAllItems();
	}

	track = tr;

	if (tr != nullptr)
	{
		tr->addSendsListener(this);
		for (TrackSendNode* send : tr->getSends())
		{
			addSend(send);
		}
	}
}

void TrackSendListView::addSend(TrackSendNode* send, int position)
{
	sendList->addItem(new TrackSendListElementView(send, track), 35, position, true);
}

void TrackSendListView::removeSend(TrackSendNode* send)
{
	for (TrackSendListElementView* sendElementView : sendList->getItems())
	{
		if (sendElementView->getTrackSendNode() == send)
		{
			sendList->removeItem(sendElementView);
			break;
		}
	}
}

void TrackSendListView::buttonClicked(Button* btn)
{
	if (track == nullptr)
		return;

	if (btn == addSendButton)
	{
		Array<ButtonList::ButtonParameters> buttons;
		buttons.add({ Track::AudioSourcePosition::PrePlugins , "Before plugins" });
		int i = 1;
		for (AudioProcessorPlayerDouble* plugin : track->getPlugins())
		{
			buttons.add({ i++, "After " + plugin->getName()});
		}
		buttons.add({ Track::AudioSourcePosition::PreFader , "Before fader" });
		buttons.add({ Track::AudioSourcePosition::PostFader , "After fader" });

		SimpleCallOutBox::open(new ButtonList(40, 250, buttons, [this](ButtonList* list, int sendPosition) {
			track->addSend(sendPosition);
			list->getParentComponent()->exitModalState(0);
		}), addSendButton->getScreenBounds());
	}
}

void TrackSendListView::sendAdded(TrackSendNode* send, int position)
{
	addSend(send, position);
}

void TrackSendListView::sendRemoved(TrackSendNode* send)
{
	removeSend(send);
}

void TrackSendListView::sendMoved(TrackSendNode*, int oldPosition, int newPosition)
{
	sendList->moveItem(oldPosition, newPosition);
}

void TrackSendListView::swapGestureCallback(FlexList<TrackSendListElementView>*, int sourceIndex, int targetIndex)
{
	track->moveSend(sourceIndex, targetIndex);
}
