#include "woap_gain_fader_view.h"
#include "woap_gain_fader.h"
#include "woap_track.h"
#include "woap_track_level_indicator_view.h"
#include "woap_parameter_float.h"
#include "woap_vertical_db_scale.h"
#include "woap_track_send_node.h"

using namespace WOAP;
using namespace WOAP::GUI;

GainFaderView::GainFaderView(Track* tr)
	: track(nullptr)
{
	levelIndicator = new TrackLevelIndicatorView();
	levelIndicator->setSkewFactor(1.5f);
	levelIndicator->setRange(-80.0f, 17.0f);
	addAndMakeVisible(levelIndicator);

	fader = new GainFader();
	fader->setThumbOpacity(0.75f);
	fader->setSkewFactor(3.0f);
	fader->setRange(-145, 10);
	fader->addListener(this);
	addAndMakeVisible(fader);

	dbScale = new VerticalDbScale();
	dbScale->setSkewFactor(3.0f);
	dbScale->setRange(-145.0f, 10.0f);
	addAndMakeVisible(dbScale);

	setTrack(tr);
}

GainFaderView::~GainFaderView()
{
	setTrack(nullptr);
}

void GainFaderView::resized()
{
	const Rectangle<int> bounds = getLocalBounds();
	const int width = getWidth();
	const int halfWidth = (int)(width * 0.5f);

	dbScale->setMargin(jmin(getHeight() / 8, 30));

	dbScale->setBounds(bounds.withLeft(halfWidth + 5));
	levelIndicator->setBounds(bounds.withWidth(halfWidth));
	fader->setBounds(bounds.withWidth(halfWidth).withSizeKeepingCentre(halfWidth - 7, getHeight()));
}

void GainFaderView::setTrack(Track* tr)
{
	if (track != nullptr)
		track->getGainParameter()->removeListener(this);
		
	track = tr;
	levelIndicator->setTrack(tr);

	if (tr != nullptr) 
	{
		ParameterFloat* gainParameter = track->getGainParameter();

		gainParameter->addListener(this);
		fader->setRange(gainParameter->getMinValue(), gainParameter->getMaxValue(), 0.01f);
		fader->setValue(gainParameter->get(), NotificationType::dontSendNotification);
	}
}

void GainFaderView::sliderValueChanged(Slider*)
{
	*track->getGainParameter() = (float)fader->getValue();
}

void GainFaderView::sliderDragStarted(Slider*)
{
	dragging = true;
}

void GainFaderView::sliderDragEnded(Slider*)
{
	dragging = false;
	fader->setValue(track->getGainParameter()->get(), NotificationType::dontSendNotification);
}

void GainFaderView::parameterChanged(Parameter*)
{
	if (!dragging)
		fader->setValue((double)track->getGainParameter()->get(), NotificationType::dontSendNotification);
}













TrackSendNodeGainFaderView::TrackSendNodeGainFaderView(TrackSendNode* trackSend)
	: trackSendNode(nullptr)
{
	fader = new GainFader();
	fader->setSkewFactor(3.0f);
	fader->setRange(-145, 10);
	fader->addListener(this);
	fader->setColour(Slider::trackColourId, Colour(70, 70, 70));
	addAndMakeVisible(fader);

	dbScale = new VerticalDbScale();
	dbScale->setSkewFactor(3.0f);
	dbScale->setRange(-145.0f, 10.0f);
	addAndMakeVisible(dbScale);

	setTrackSendNode(trackSend);
}

TrackSendNodeGainFaderView::~TrackSendNodeGainFaderView()
{
	setTrackSendNode(nullptr);
}

void TrackSendNodeGainFaderView::resized()
{
	const Rectangle<int> bounds = getLocalBounds();
	const int width = getWidth();
	const int halfWidth = (int)(width * 0.5f);

	dbScale->setMargin(getHeight() / 8);

	dbScale->setBounds(bounds.withLeft(halfWidth + 5));
	fader->setBounds(bounds.withWidth(halfWidth).withSizeKeepingCentre(halfWidth - 7, getHeight()));
}

void TrackSendNodeGainFaderView::setTrackSendNode(TrackSendNode* trackSend)
{
	if (trackSendNode != nullptr)
		trackSendNode->getGainParameter()->removeListener(this);

	trackSendNode = trackSend;

	if (trackSend != nullptr) 
	{
		ParameterFloat* gainParameter = trackSendNode->getGainParameter();

		gainParameter->addListener(this);
		fader->setRange(gainParameter->getMinValue(), gainParameter->getMaxValue(), 0.01f);
		fader->setValue(gainParameter->get(), NotificationType::dontSendNotification);
	}
}

void TrackSendNodeGainFaderView::sliderValueChanged(Slider*)
{
	*trackSendNode->getGainParameter() = (float)fader->getValue();
}

void TrackSendNodeGainFaderView::sliderDragStarted(Slider*)
{
	dragging = true;
}

void TrackSendNodeGainFaderView::sliderDragEnded(Slider*)
{
	dragging = false;
	fader->setValue(trackSendNode->getGainParameter()->get(), NotificationType::dontSendNotification);
}

void TrackSendNodeGainFaderView::parameterChanged(Parameter*)
{
	if (!dragging)
		fader->setValue((double)trackSendNode->getGainParameter()->get(), NotificationType::dontSendNotification);
}