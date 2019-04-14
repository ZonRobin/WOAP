#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class MainComponent;
class PluginParameter;

class ParameterAutomizer : public Thread, public Timer
{
public:
	ParameterAutomizer(int parameterIndexToAutomate);
	~ParameterAutomizer();

	virtual void run() override;
	virtual void timerCallback() override;

protected:
	int parameterIndex;
	float currentValue = 0;
	bool goesUp = true;
	MainComponent* main;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterAutomizer)
};