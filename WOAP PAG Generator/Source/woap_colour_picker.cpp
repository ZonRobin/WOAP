#include "woap_colour_picker.h"

ColourPicker::ColourPicker()
{
	colourPreview = new ColourPreview();
	addChildComponent(colourPreview);
}

ColourPicker::~ColourPicker()
{

}

void ColourPicker::resized()
{
	colourPreview->setBounds(0, 0, colourPreviewSize, colourPreviewSize);
}

void ColourPicker::mouseMove(const MouseEvent& event)
{
	const Point<int> position = event.getPosition();
	const Rectangle<int> drawnArea(position.x, position.y, 1, 1);
	const Colour colourSelected = getParentComponent()->createComponentSnapshot(drawnArea).getPixelAt(0, 0);

	colourPreview->setActiveColour(colourSelected);
	listeners.call(&ColourPicker::Listener::hoveredOverColour, colourSelected);

	colourPreview->setBounds(position.x + 2, position.y - 2 - colourPreviewSize, colourPreviewSize, colourPreviewSize);
}

void ColourPicker::mouseDown(const MouseEvent & event)
{
	const Point<int> position = event.getPosition();
	const Rectangle<int> drawnArea(position.x, position.y, 1, 1);
	const Colour colourSelected = getParentComponent()->createComponentSnapshot(drawnArea).getPixelAt(0, 0);

	colourPreview->setActiveColour(colourSelected);
	listeners.call(&ColourPicker::Listener::colourPicked, colourSelected);
}

void ColourPicker::mouseEnter(const MouseEvent&)
{
	colourPreview->setVisible(true);
}

void ColourPicker::mouseExit(const MouseEvent&)
{
	colourPreview->setVisible(false);
}

void ColourPicker::paint(Graphics& g)
{
	const Rectangle<int> bounds = getLocalBounds();
	const Rectangle<float> fbounds = bounds.toFloat();

	g.setColour(Colours::lightblue.withAlpha(0.8f));

	const float dashLengths[] = { jmin(getWidth() / 5.0f, 8.0f), jmin(getWidth() / 5.0f, 8.0f) };
	g.drawDashedLine(Line<float>(fbounds.getTopLeft(), fbounds.getTopRight()), dashLengths, 2, 2.0f);
	g.drawDashedLine(Line<float>(fbounds.getTopRight(), fbounds.getBottomRight()), dashLengths, 2, 2.0f);
	g.drawDashedLine(Line<float>(fbounds.getBottomRight(), fbounds.getBottomLeft()), dashLengths, 2, 2.0f);
	g.drawDashedLine(Line<float>(fbounds.getBottomLeft(), fbounds.getTopLeft()), dashLengths, 2, 2.0f);

	g.drawFittedText("Pick a color", getLocalBounds().withHeight(25), Justification::centred, 1);
}

void ColourPicker::setColourPreviewSize(int newSize)
{
	jassert(newSize > 0);

	colourPreviewSize = newSize; 
	resized();
}
