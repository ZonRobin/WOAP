#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Window : public DocumentWindow
{
public:
	class CloseWindowListener
	{
	public:
		virtual void closeWindowTriggered(Window* closedWindow) = 0;
	};

	Window(CloseWindowListener* own, const String& name,
		Colour backgroundColour,
		bool escapeKeyTriggersCloseButton,
		bool addToDesktop = true);
	virtual ~Window();

	void closeButtonPressed() override;

private:
	CloseWindowListener* owner;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Window)
};
