#include "woap_track_io_selector.h"
#include "woap_mixer_node_selector.h"
#include "woap_track.h"
#include "woap_simple_call_out_box.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackIOSelector::TrackIOSelector(Type t, Track* tr)
	: type(t)
{
	if (type == Type::Input)
		selectorButton = new TextButton("Input");
	else
		selectorButton = new TextButton("Output");

	selectorButton->setMouseCursor(MouseCursor::PointingHandCursor);
	selectorButton->addListener(this);
	addAndMakeVisible(selectorButton);

	setTrack(tr);
}

TrackIOSelector::~TrackIOSelector()
{
	SimpleCallOutBox::close(selectorCallOut);
}

void TrackIOSelector::setTrack(Track* tr)
{
	track = tr;
}

void TrackIOSelector::resized()
{
	selectorButton->setBounds(getLocalBounds());
}

void TrackIOSelector::buttonClicked(Button*)
{
	Component* selector;

	if (type == Type::Input)
		selector = new MixerInputNodeSelector(120, track);
	else
		selector = new MixerOutputNodeSelector(120, track);
	
	selectorCallOut = SimpleCallOutBox::open(selector, getScreenBounds());
}
