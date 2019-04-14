#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class WOAPLookAndFeel : public LookAndFeel_V4
{
public:
	WOAPLookAndFeel();
	~WOAPLookAndFeel();

	void drawButtonBackground(Graphics& g,
		Button& button,
		const Colour& backgroundColour,
		bool isMouseOverButton,
		bool isButtonDown) override;

	Button* createDocumentWindowButton(int) override;
	void drawLabel(Graphics& g, Label& label) override;

	static Colour getTextColour()
	{
		return LookAndFeel::getDefaultLookAndFeel().findColour(Label::ColourIds::textColourId);
	}

	static Colour getHeaderBackgroundColour()
	{
		return Colour(33, 33, 33).brighter();
	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WOAPLookAndFeel)
};

struct CustomColours
{
	static const Colour red;
	static const Colour orange;
	static const Colour yellow;
	static const Colour lightGreen;
	static const Colour green;
	static const Colour lightBlue;
	static const Colour blue;
	static const Colour violet;
	static const Colour grey;
	static const Colour black;
	static const Colour white;

	static const Colour audioTrackColour;
	static const Colour mixBusTrackColour;
	static const Colour fxTrackColour;
	static const Colour outputBusTrackColour;
	static const Colour lightTextColour;
	static const Colour darkTextColour;

	static const Array<Colour> trackColourPalette;
};