#include "woap_gain_fader.h"

using namespace WOAP;
using namespace WOAP::GUI;

GainFader::GainFader()
	: Slider(Slider::SliderStyle::LinearVertical, Slider::TextEntryBoxPosition::NoTextBox)
{
	setMouseCursor(MouseCursor::PointingHandCursor);
	setRepaintsOnMouseActivity(false);
	setColour(Slider::trackColourId, Colour(100, 100, 100).withAlpha(0.5f));
	setSkewFactor(3.0f);
	setViewportIgnoreDragFlag(true);
	setVelocityModeParameters(0.2, 0, 0.175);
	setVelocityBasedMode(true);
}


GainFader::~GainFader()
{

}

void GainFader::paint(Graphics& g)
{
	const Rectangle<int> bounds = getLocalBounds();

	g.setColour(findColour(Slider::trackColourId));
	g.fillRect(bounds.withSizeKeepingCentre(2, bounds.getHeight()));

	paintThumb(g);
}

void GainFader::paintThumb(Graphics& g)
{
	const Rectangle<int> bounds = getThumbPosition();

	g.setColour(Colour(40, 40, 40).withAlpha(thumbOpacity));
	g.fillRoundedRectangle(bounds.toFloat(), 3.0f);

	g.setColour(Colour(230, 230, 230));
	g.fillRect(bounds.getX() + 1, bounds.getY() + (int)(bounds.getHeight() / 2.0f), bounds.getWidth() - 2, 2);

	g.setColour(Colour(100, 100, 100));
	g.fillRect(bounds.getX() + 3, bounds.getY() + (int)(bounds.getHeight() * 0.35f), bounds.getWidth() - 6, 1);
	g.fillRect(bounds.getX() + 3, bounds.getY() + (int)(bounds.getHeight() * 0.65f), bounds.getWidth() - 6, 1);

	g.fillRect(bounds.getX() + 3, bounds.getY() + (int)(bounds.getHeight() * 0.2f), bounds.getWidth() - 6, 1);
	g.fillRect(bounds.getX() + 3, bounds.getY() + (int)(bounds.getHeight() * 0.8f), bounds.getWidth() - 6, 1);
}


Rectangle<int> GainFader::getThumbPosition()
{
	const Rectangle<int> bounds = getLocalBounds();
	const int thumbHeight = jmin((int)(bounds.getHeight() / 4.0f), 60);
	const int thumbPosY = (int)((float)(1 - valueToProportionOfLength(getValue())) * (bounds.getHeight() - thumbHeight));

	return Rectangle<int>(0, thumbPosY, bounds.getWidth(), thumbHeight);
}

void GainFader::mouseDown(const MouseEvent& e)
{
	if (getThumbPosition().contains(e.getPosition())) {
		mouseDownOnThumb = true;
		Slider::mouseDown(e);
	}
}

void GainFader::mouseDrag(const MouseEvent& e)
{
	if (mouseDownOnThumb)
		Slider::mouseDrag(e);
}

void GainFader::mouseUp(const MouseEvent& e)
{
	if (mouseDownOnThumb)
		Slider::mouseUp(e);

	mouseDownOnThumb = false;
}
