#include "woap_track_send_node_detail_view.h"
#include "woap_track_send_node.h"
#include "woap_track.h"
#include "woap_gain_fader_view.h"
#include "woap_pan_view.h"
#include "woap_track_send_node_source_position_view.h"
#include "woap_name_label.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackSendNodeDetailView::TrackSendNodeDetailView(TrackSendNode* send, Track* sendOwner) 
	: trackSendNode(send), track(sendOwner)
{
	nameLabel = new MixerNodeNameLabel(true, send);
	nameLabel->setFontSize(18.0f);
	addAndMakeVisible(nameLabel);

	audioSourcePositionView = new TrackSendNodeSourcePositionView(track, trackSendNode);
	addAndMakeVisible(audioSourcePositionView);

	panFaderView = new PanView(send->getPanParameter());
	addAndMakeVisible(panFaderView);

	gainFaderView = new TrackSendNodeGainFaderView(send);
	addAndMakeVisible(gainFaderView);
}

TrackSendNodeDetailView::~TrackSendNodeDetailView()
{
}

void TrackSendNodeDetailView::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	nameLabel->setBounds(10, 10, getWidth() - 20, 30);
	audioSourcePositionView->setBounds(10, 50, bounds.getWidth() - 20, 30);

	panFaderView->setBounds(60, 90, 80, 30);
	gainFaderView->setBounds(80, 130, 80, 240);
}

void TrackSendNodeDetailView::paint(Graphics& g)
{
	g.fillAll(Colour(0xFF6A6A6A).interpolatedWith(track->getColour(), 0.1f));
}