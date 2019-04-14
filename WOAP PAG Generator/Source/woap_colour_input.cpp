#include "woap_colour_input.h"

ColourInput* ColourInput::activePickingInput = nullptr;

ColourInput::ColourInput()
{
	pickerButton = new TextButton("Pick", "Pick the colour from the view");
	pickerButton->addListener(this);
	addChildComponent(pickerButton);
}

ColourInput::~ColourInput()
{
	if (colourPicker != nullptr)
	{
		colourPicker->removeListener(this);
	}

	if (activePickingInput == this)
	{
		activePickingInput = nullptr;
	}
}

void ColourInput::setColourPickerToUse(ColourPicker* cp)
{
	colourPicker = cp;

	if (colourPicker != nullptr)
	{
		pickerButton->setVisible(true);
	}
}

void ColourInput::paint(Graphics& g)
{
	g.fillAll(activeColour);

	g.setColour(Colour((uint8)255, 255, 255, 0.3f));
	g.drawRect(getLocalBounds());
}

void ColourInput::resized()
{
	pickerButton->setBounds(getLocalBounds().withTop((int)(getLocalBounds().getHeight() * 0.65f)));
}

void ColourInput::setActiveColour(const Colour& c, bool notify)
{
	activeColour = c;

	if (notify)
	{
		listeners.call(&ColourInput::Listener::colourSelected, this, c);
	}
	
	repaint();
}

void ColourInput::mouseUp(const MouseEvent&)
{
	colourSelector = new ColourSelector((ColourSelector::showColourAtTop | ColourSelector::showSliders | ColourSelector::showColourspace));
	colourSelector->setCurrentColour(activeColour);
	colourSelector->setBounds(0, 0, 300, 300);
	colourSelector->addChangeListener(this);

	CallOutBox::launchAsynchronously(colourSelector, getScreenBounds(), nullptr);
}

void ColourInput::changeListenerCallback(ChangeBroadcaster*)
{
	activeColour = colourSelector->getCurrentColour();
	listeners.call(&ColourInput::Listener::colourSelected, this, activeColour);
	repaint();
}

void ColourInput::hoveredOverColour(Colour colour)
{
	activeColour = colour;
	listeners.call(&ColourInput::Listener::colourSelected, this, activeColour);
	repaint();
}

void ColourInput::colourPicked(Colour colour)
{
	stopPickingColour();

	activeColour = colour;
	listeners.call(&ColourInput::Listener::colourSelected, this, activeColour);
	repaint();
}

void ColourInput::buttonClicked(Button*)
{
	if (colourPicker != nullptr)
	{
		if (activePickingInput == this)
		{
			activePickingInput->stopPickingColour();
			return;
		}
		else if (activePickingInput != nullptr)
		{
			activePickingInput->stopPickingColour();
		}

		colourPicker->addListener(this);
		colourPicker->setVisible(true);
		pickerButton->setToggleState(true, NotificationType::sendNotification);
		activePickingInput = this;
		
	}
}

void ColourInput::stopPickingColour()
{
	if (colourPicker != nullptr)
	{
		colourPicker->setVisible(false);
		colourPicker->removeListener(this);
		pickerButton->setToggleState(false, NotificationType::sendNotification);

		if (activePickingInput == this)
		{
			activePickingInput = nullptr;
		}
	}

	colourSelector = nullptr;
}
