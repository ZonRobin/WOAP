#include "woap_panner.h"
#include "woap_parameter_float.h"

using namespace WOAP;
using namespace WOAP::GUI;

Panner::Panner() 
	: Slider(SliderStyle::LinearHorizontal, TextEntryBoxPosition::NoTextBox)
{
	setViewportIgnoreDragFlag(true);
	setVelocityModeParameters(0.2, 0, 0.25);
	setVelocityBasedMode(true);
}


Panner::~Panner()
{

}

void Panner::paint(Graphics& g)
{
	const Rectangle<float> bounds = getLocalBounds().toFloat();
	
	g.setColour(findColour(Slider::ColourIds::backgroundColourId));
	g.fillRoundedRectangle(bounds, 3.0f);

	const float centerX = bounds.getWidth() / 2;
	const float valueX = (float)valueToProportionOfLength(getValue()) * bounds.getWidth();

	Path valuePath;
	if (valueX > centerX)
		valuePath.addRoundedRectangle(centerX, 0, valueX - centerX, bounds.getHeight(), 3.0f, 3.0f, false, true, false, true);
	else
		valuePath.addRoundedRectangle(valueX, 0, centerX - valueX, bounds.getHeight(), 3.0f, 3.0f, true, false, true, false);

	g.setColour(findColour(Slider::ColourIds::trackColourId));
	g.fillPath(valuePath);

	g.setColour(Colour(200, 200, 200));
	g.fillRect((int)centerX - 1, 0, 2, (int)bounds.getHeight());
}
