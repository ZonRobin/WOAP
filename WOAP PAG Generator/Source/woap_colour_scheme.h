#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

struct ColourScheme
{
	Colour accent;
	Colour fill;

	ColourScheme()
	{

	}

	ColourScheme(Colour accentToUse, Colour fillToUse)
		: accent(accentToUse), fill(fillToUse)
	{

	}
};