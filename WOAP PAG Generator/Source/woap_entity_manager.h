#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent;

class EntityManager
{
public:
	static void setMainComponent(MainComponent* main) { mainComponent = main; };
	static inline MainComponent* getMainComponent() { return mainComponent; }

protected:
	static MainComponent* mainComponent;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EntityManager)
};