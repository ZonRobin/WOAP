#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ColourPreview;

class ColourPicker : public Component
{
public:
	class Listener
	{
	public:
		virtual void hoveredOverColour(Colour colour) = 0;
		virtual void colourPicked(Colour colour) = 0;
	};

	ColourPicker();
	~ColourPicker();

	virtual void resized();
	virtual void mouseMove(const MouseEvent& event) override;
	virtual void mouseDown(const MouseEvent& event) override;
	virtual void mouseEnter(const MouseEvent& event) override;
	virtual void mouseExit(const MouseEvent& event) override;

	virtual void paint(Graphics& g) override;

	void addListener(Listener* listenerToAdd) { listeners.add(listenerToAdd); }
	void removeListener(Listener* listenerToRemove) { listeners.remove(listenerToRemove); }
	
	bool isInUse() const { return !listeners.isEmpty(); }
	void setColourPreviewSize(int newSize);

protected:
	class ColourPreview : public Component
	{
	public:
		ColourPreview() {}
		~ColourPreview() {}

		void setActiveColour(Colour newColour) { activeColour = newColour; repaint(); }
		virtual void paint(Graphics& g) 
		{
			g.fillAll(activeColour);
			g.setColour(Colours::green); 
			g.drawRect(getLocalBounds()); 
		}

	protected:
		Colour activeColour;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourPreview)
	};

	ListenerList<Listener> listeners;
	ScopedPointer<ColourPreview> colourPreview;
	int colourPreviewSize = 30;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourPicker)
	JUCE_DECLARE_WEAK_REFERENCEABLE(ColourPicker)
};