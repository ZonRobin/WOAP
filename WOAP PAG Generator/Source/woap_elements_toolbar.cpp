#include "woap_elements_toolbar.h"
#include "woap_entity_manager.h"
#include "woap_main_component.h"
#include "woap_view_editor.h"
#include "woap_look_and_feel.h"

ElementsToolbar::ElementsToolbar()
{
	ellipse = new BlankEllipse();
	ellipse->setDragAndDropEnabled(true);
	addAndMakeVisible(ellipse);

	rectangle = new BlankRectangle();
	rectangle->setDragAndDropEnabled(true);
	addAndMakeVisible(rectangle);

	colourInput = new ColourInput();
	colourInput->addListener(this);
	colourInput->setActiveColour(CustomColours::lightGreen);
	colourInput->setColourPickerToUse(EntityManager::getMainComponent()->getViewEditor()->getColourPicker());
	addAndMakeVisible(colourInput);

	EntityManager::getMainComponent()->getViewEditor()->addListener(this);
}

ElementsToolbar::~ElementsToolbar()
{
	EntityManager::getMainComponent()->getViewEditor()->removeListener(this);
}

void ElementsToolbar::paint(Graphics & g)
{
	const Rectangle<int> bounds = getLocalBounds();
	const Colour text = WOAPLookAndFeel::getTextColour();
	const Colour highlight = WOAPLookAndFeel::getHeaderBackgroundColour();

	g.setColour(text);
	g.drawFittedText("Drag&drop element to the editor area", bounds.withHeight(30), Justification::centred, 1);

	g.setColour(text);
	g.drawFittedText("Color", 70, 90, 100, 30, Justification::centred, 1);
}

void ElementsToolbar::resized()
{
	const Rectangle<int> bounds = getLocalBounds();
	const int elementOffsetX = (bounds.getWidth() - 100) / 2;

	rectangle->setBounds(elementOffsetX, 30, 40, 40);
	ellipse->setBounds(elementOffsetX + 60, 30, 40, 40);

	colourInput->setBounds(150, 90, 40, 40);
}

void ElementsToolbar::colourSelected(ColourInput*, const Colour& colour)
{
	ellipse->setActiveColour(colour);
	rectangle->setActiveColour(colour);

	if (activeElement != nullptr)
	{
		activeElement->setActiveColour(colour);
	}
}

void ElementsToolbar::elementTouched(ViewEditorElement* element)
{
	if (element == ellipse || element == rectangle || element == nullptr)
	{
		activeElement = nullptr;
	}
	else
	{
		activeElement = element;
		colourInput->setActiveColour(element->getActiveColour());
	}
}

void ElementsToolbar::mouseDown(const MouseEvent&)
{
	if (activeElement != nullptr)
	{
		activeElement->setFocused(false);
		activeElement = nullptr;
	}
}
