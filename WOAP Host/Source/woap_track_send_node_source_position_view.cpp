#include "woap_track_send_node_source_position_view.h"
#include "woap_track.h"
#include "woap_audio_processor_player_double.h"
#include "woap_button_list.h"
#include "woap_simple_call_out_box.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackSendNodeSourcePositionView::TrackSendNodeSourcePositionView(Track* tr, TrackSendNode* sendNode)
	: track(tr), trackSendNode(sendNode)
{
	trackSendNode->addAudioSourcePositionListener(this);

	updateText();
}

TrackSendNodeSourcePositionView::~TrackSendNodeSourcePositionView()
{
	trackSendNode->removeAudioSourcePositionListener(this);
}

void TrackSendNodeSourcePositionView::updateText()
{
	const int sendPosition = trackSendNode->getAudioSourcePosition();
	switch (sendPosition)
	{
	case Track::AudioSourcePosition::PrePlugins:
		setButtonText("Before plugins");
		return;
	case Track::AudioSourcePosition::PreFader:
		setButtonText("Before fader");
		return;
	case Track::AudioSourcePosition::PostFader:
		setButtonText("After fader");
		return;
	default:
	{
		if (sendPosition < 1 || sendPosition > track->getPlugins().size())
			return;

		setButtonText("After " + track->getPlugins()[sendPosition - 1]->getName());
	}
	}
}

void TrackSendNodeSourcePositionView::clicked()
{
	Array<ButtonList::ButtonParameters> buttons;
	buttons.add({ Track::AudioSourcePosition::PrePlugins , "Before plugins" });
	int i = 1;
	for (AudioProcessorPlayerDouble* plugin : track->getPlugins())
	{
		buttons.add({ i++, "After " + plugin->getName() });
	}
	buttons.add({ Track::AudioSourcePosition::PreFader , "Before fader" });
	buttons.add({ Track::AudioSourcePosition::PostFader , "After fader" });

	SimpleCallOutBox::open(new ButtonList(40, 250, buttons, [this](ButtonList* list, int sendPosition) {
		trackSendNode->setAudioSourcePosition(sendPosition);
		list->getParentComponent()->exitModalState(0);
	}), getScreenBounds());
}

void TrackSendNodeSourcePositionView::sendAudioSourcePositionChanged(TrackSendNode*)
{
	updateText();
}