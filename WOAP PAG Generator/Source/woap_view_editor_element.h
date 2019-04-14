#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_view_editor_node.h"

class ViewEditorElement : public ViewEditorNode
{
public:
	ViewEditorElement() {}
	ViewEditorElement(const ViewEditorElement& other) { activeColour = other.activeColour; }
	~ViewEditorElement() {}

	void setActiveColour(Colour newColour) { activeColour = newColour; repaint(); }
	Colour getActiveColour() { return activeColour; }

protected:
	Colour activeColour;

private:
	JUCE_LEAK_DETECTOR(ViewEditorElement)
};