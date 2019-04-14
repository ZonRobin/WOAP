#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class LoadingScreenComponent : public Component
{
public:
	LoadingScreenComponent();
	virtual ~LoadingScreenComponent();

	virtual void paint(Graphics& g) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoadingScreenComponent)
};