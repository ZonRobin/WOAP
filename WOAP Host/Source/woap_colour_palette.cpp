#include "woap_colour_palette.h"
#include "woap_look_and_feel.h"

using namespace WOAP;
using namespace WOAP::GUI;

ColourPalette::ColourPalette()
{
	colourNodeList = new FlexList<ColourPalette::Node>(FlexList<ColourPalette::Node>::Direction::Vertical);
	addAndMakeVisible(colourNodeList);

	for (Colour c : CustomColours::trackColourPalette)
	{
		colourNodeList->addItem(new Node(c, this), 50, false);
	}

	setSize(200, 50 * CustomColours::trackColourPalette.size());
}

ColourPalette::~ColourPalette()
{
}

void ColourPalette::resized()
{
	colourNodeList->setBounds(getLocalBounds());
}

void ColourPalette::nodeClicked(Node* node)
{
	listeners.call(&Listener::paletteColourSelected, node->getColour());
}