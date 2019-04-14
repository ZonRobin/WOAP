#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_blank_ellipse.h"
#include "woap_blank_rectangle.h"
#include "woap_colour_input.h"
#include "woap_view_editor.h"

class ElementsToolbar : public Component, public ColourInput::Listener, public ViewEditor::Listener
{
public:
	ElementsToolbar();
	~ElementsToolbar();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;
	virtual void colourSelected(ColourInput* input, const Colour& colour) override;

	virtual void mouseDown(const MouseEvent& e) override;
	virtual void elementTouched(ViewEditorElement* element) override;

protected:
	ScopedPointer<BlankEllipse> ellipse;
	ScopedPointer<BlankRectangle> rectangle;
	ScopedPointer<ColourInput> colourInput;

	ViewEditorElement* activeElement = nullptr;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ElementsToolbar)
};