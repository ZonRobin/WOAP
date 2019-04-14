#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ComponentVisibilitor
{
public:
	ComponentVisibilitor(Component* componentToHandle);
	virtual ~ComponentVisibilitor();

	void setActive(bool newActive) { active = newActive; }

protected:
	Component* component;
	bool active = true;

	static Array<ComponentVisibilitor*> visibilitors;

private:
	JUCE_LEAK_DETECTOR(ComponentVisibilitor)
};