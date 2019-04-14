#include "woap_track_view.h"
#include "woap_track.h"
#include "woap_audio_core.h"
#include "woap_track_plugin_list_view.h"
#include "woap_track_send_list_view.h"
#include "woap_gain_fader_view.h"
#include "woap_name_label.h"
#include "woap_button_view.h"
#include "woap_pan_view.h"
#include "woap_main_view.h"
#include "woap_drag_handle.h"
#include "woap_main_view.h"
#include "woap_audio_core_view.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackView::TrackView(Track* t) : track(t)
{
	pluginListView = new TrackPluginListView(true, track);
	pluginListView->setBounds(10, 10, 80, 210);
	addAndMakeVisible(pluginListView);

	nameLabel = new MixerNodeNameLabel(false, track);
	nameLabel->setBounds(10, 230, 80, 30);
	nameLabel->addMouseListener(this, false);
	addAndMakeVisible(nameLabel);
	
	muteButton = new MuteButtonView("Mute", track);
	muteButton->setBounds(10, 265, 80, 30);
	addAndMakeVisible(muteButton);

	soloButton = new SoloButtonView("Solo", track);
	soloButton->setBounds(10, 300, 80, 30);
	addAndMakeVisible(soloButton);

	panFader = new PanView(track->getPanParameter());
	panFader->setBounds(10, 335, 80, 25);
	addAndMakeVisible(panFader);

	gainFader = new GainFaderView(track);
	gainFader->setBounds(15, 370, 75, 180);
	addAndMakeVisible(gainFader);

	dragHandle = new DragHandle();
	dragHandle->addMouseListener(this, true);
	dragHandle->setBounds(10, 554, 80, 30);
	addAndMakeVisible(dragHandle);

	track->addListener(this);
}

TrackView::~TrackView()
{
	track->removeListener(this);
}

void TrackView::selfDelete() const
{
	MessageManager::callAsync([this]() {
		AudioCore::getInstance()->removeTrack(track);
	});
}

void TrackView::paint(Graphics& g)
{
	Colour backgroundColour = Colours::grey.interpolatedWith(track->getColour(), 0.10f);
	g.setColour(isSelected ? backgroundColour.brighter() : backgroundColour);
	g.fillRoundedRectangle(getLocalBounds().reduced(3, 0).toFloat(), 3.0f);
	
	g.setColour(isSelected ? Colour(230, 230, 230) : Colours::grey.darker());
	g.drawRoundedRectangle(getLocalBounds().reduced(3, 0).toFloat(), 3.0f, isSelected ? 2.0f : 1);
}

void TrackView::mouseDrag(const MouseEvent& ev)
{
	if (!startDrag)
		return;

	if (ev.eventComponent == dragHandle)
	{
		startDrag = false;
		MainView::getInstance()->startDragging(var(), this);
	}
}

void TrackView::mouseDown(const MouseEvent& ev)
{
	startDrag = ev.eventComponent == dragHandle;
}

void TrackView::mouseUp(const MouseEvent& ev)
{
	startDrag = false;
	if (ev.eventComponent == nameLabel || ev.eventComponent == dragHandle)
	{
		if (ev.mods.isLeftButtonDown() && !ev.source.hasMouseMovedSignificantlySincePressed())
		{
			MainView::getInstance()->getAudioCoreView()->setSelectedTrack(track);
		}
	}
}

void TrackView::setSelected(bool selected)
{
	if (selected == isSelected)
		return;

	isSelected = selected; 
	repaint();
}
