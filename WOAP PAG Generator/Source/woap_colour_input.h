#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_colour_picker.h"

class ColourInput : public Component, public ChangeListener, public ColourPicker::Listener, public Button::Listener
{
public:
	class Listener
	{
	public:
		virtual void colourSelected(ColourInput* input, const Colour& colour) = 0;
	};

	ColourInput();
	~ColourInput();

	void setColourPickerToUse(ColourPicker* cp);

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

	void setActiveColour(const Colour& c, bool notify = true);
	Colour getActiveColour() const { return activeColour; }

	void addListener(Listener* listenerToAdd) { listeners.add(listenerToAdd); }
	void removeListener(Listener* listenerToRemove) { listeners.remove(listenerToRemove); }

	virtual void mouseUp(const MouseEvent& event) override;
	virtual void changeListenerCallback(ChangeBroadcaster* source) override;

	virtual void hoveredOverColour(Colour colour) override;
	virtual void colourPicked(Colour colour) override;

	virtual void buttonClicked(Button*) override;

	void stopPickingColour();

protected:
	Colour activeColour;
	ListenerList<Listener> listeners;
	ColourSelector* colourSelector = nullptr;
	WeakReference<ColourPicker> colourPicker;
	ScopedPointer<TextButton> pickerButton;

	static ColourInput* activePickingInput;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourInput)
};