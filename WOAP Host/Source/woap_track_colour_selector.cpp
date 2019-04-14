#include "woap_track_colour_selector.h"
#include "woap_colour_palette.h"
#include "woap_simple_call_out_box.h"

using namespace WOAP;
using namespace WOAP::GUI;

TrackColourSelector::TrackColourSelector(Track* track)
{
	selectColourButton = new TextButton("Color");
	selectColourButton->addListener(this);
	selectColourButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(selectColourButton);

	setTrack(track);
}

TrackColourSelector::~TrackColourSelector()
{
	SimpleCallOutBox::close(colourPaletteCallOut);
}

void TrackColourSelector::setTrack(Track* tr)
{
	if (track != nullptr)
		track->removeListener(this);

	track = tr;

	if (tr != nullptr)
		tr->addListener(this);
}

void TrackColourSelector::paint(Graphics&)
{
}

void TrackColourSelector::resized()
{
	selectColourButton->setBounds(getLocalBounds());
}

void TrackColourSelector::aboutToDelete(MixerNode*)
{
	setTrack(nullptr);
}

void TrackColourSelector::buttonClicked(Button*)
{
	openPalette();
}

void TrackColourSelector::paletteColourSelected(Colour colour)
{
	SimpleCallOutBox::close(colourPaletteCallOut);
	track->setColour(colour);
}

void TrackColourSelector::openPalette()
{
	ColourPalette* palette = new ColourPalette();
	palette->addListener(this);
	
	colourPaletteCallOut = SimpleCallOutBox::open(palette, getScreenBounds());
}