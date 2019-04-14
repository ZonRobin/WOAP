#include "woap_track_detail_view.h"
#include "woap_name_label.h"
#include "woap_track_plugin_list_view.h"
#include "woap_track_send_list_view.h"
#include "woap_track_colour_selector.h"
#include "woap_track_io_selector.h"
#include "woap_button_view.h"
#include "woap_pan_view.h"
#include "woap_gain_fader_view.h"
#include "woap_main_view.h"

using namespace WOAP;
using namespace WOAP::GUI;


TrackDetailView::TrackDetailView()
	: track(nullptr)
{
	trackNameLabel = new MixerNodeNameLabel(true);
	trackNameLabel->setFontSize(18.0f);
	addAndMakeVisible(trackNameLabel);

	trackColourSelector = new TrackColourSelector();
	addAndMakeVisible(trackColourSelector);

	trackPluginListView = new TrackPluginListView(false);
	addAndMakeVisible(trackPluginListView);

	trackSendListView = new TrackSendListView();
	addAndMakeVisible(trackSendListView);

	inputNodeSelector = new TrackIOSelector(TrackIOSelector::Type::Input);
	addAndMakeVisible(inputNodeSelector);

	outputNodeSelector = new TrackIOSelector(TrackIOSelector::Type::Output);
	addAndMakeVisible(outputNodeSelector);

	muteButton = new MuteButtonView("Mute");
	addAndMakeVisible(muteButton);

	soloButton = new SoloButtonView("Solo");
	addAndMakeVisible(soloButton);

	panFader = new PanView();
	addAndMakeVisible(panFader);

	gainFader = new GainFaderView();
	addAndMakeVisible(gainFader);
}

TrackDetailView::~TrackDetailView()
{
	setTrack(nullptr);
}

void TrackDetailView::paint(Graphics& g)
{
	Path bg;
	bg.addRoundedRectangle(0, 0, (float)getWidth(), (float)getHeight(), 3.0f, 3.0f, true, false, true, false);
	
	g.setColour(track == nullptr ? Colours::grey : Colours::grey.interpolatedWith(track->getColour(), 0.10f));
	g.fillPath(bg);
}

void TrackDetailView::resized()
{
	const Rectangle<int> bounds = getLocalBounds();
	const int width = getWidth();

	trackNameLabel->setBounds(10, 10, width - 20, 30);
	trackColourSelector->setBounds(width / 2 - 30, 50, 60, 30);

	inputNodeSelector->setBounds(10, 50, width / 3, 30);
	outputNodeSelector->setBounds(width - 10 - width / 3, 50, width / 3, 30);

	trackPluginListView->setBounds(10, 90, width - 20, 180);
	trackSendListView->setBounds(10, 280, width - 20, 180);

	muteButton->setBounds(10, 470, 80, 30);
	soloButton->setBounds(10, 510, 80, 30);
	panFader->setBounds(10, 550, 80, 30);

	if (getHeight() < 700) {
		gainFader->setVisible(false);
	} else {
		gainFader->setVisible(true);
		gainFader->setBounds(15, 590, 75, getHeight() - 600);
	}
}

void TrackDetailView::setTrack(Track* tr)
{
	if (track != nullptr)
		track->removeListener(this);
		
	track = tr;
	
	if (tr != nullptr)
		tr->addListener(this);
	
	trackChanged();
}

void TrackDetailView::trackChanged()
{
	setVisible(track != nullptr);

	trackNameLabel->setMixerNode(track);
	trackColourSelector->setTrack(track);
	trackPluginListView->setTrack(track);
	trackSendListView->setTrack(track);
	inputNodeSelector->setTrack(track);
	outputNodeSelector->setTrack(track);
	muteButton->setTrack(track);
	soloButton->setTrack(track);

	if (track != nullptr) 
		panFader->setParameter(track->getPanParameter());
	else
		panFader->setParameter(nullptr);
	
	gainFader->setTrack(track);

	if (track != nullptr)
		nameChanged(track);

	repaint();
}

void TrackDetailView::nameChanged(MixerNode*)
{
	trackNameLabel->setText(track->getName(), NotificationType::dontSendNotification);
}

void TrackDetailView::colourChanged(MixerNode *)
{
	repaint();
}
