#include "woap_parameter_automizer.h"
#include "woap_entity_manager.h"
#include "woap_main_component.h"

ParameterAutomizer::ParameterAutomizer(int parameterIndexToAutomate)
	: Thread("Parameter " + String(parameterIndexToAutomate) + " automizer"), parameterIndex(parameterIndexToAutomate)
{
	main = EntityManager::getMainComponent();
	startThread();
}

ParameterAutomizer::~ParameterAutomizer()
{
	stopThread(100);
}

void ParameterAutomizer::run()
{
	currentValue = 0;
	goesUp = true;
	startTimerHz(50);
}

void ParameterAutomizer::timerCallback()
{
	if (threadShouldExit())
	{
		stopTimer();
		return;
	}

	if (goesUp)
	{
		currentValue += 0.05f;
		if (currentValue >= 1.0f) goesUp = false;
	}
	else
	{
		currentValue -= 0.05f;
		if (currentValue <= 0) goesUp = true;
	}

	main->setParameterValue(parameterIndex, currentValue);
}
