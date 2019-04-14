#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_view_editor_element.h"
#include "woap_colour_scheme.h"

class BlankRectangle : public ViewEditorElement
{
public:
	BlankRectangle();
	BlankRectangle(const BlankRectangle& other);
	~BlankRectangle() {}

	virtual void paint(Graphics& g) override;
	
	virtual XmlElement* createXml() const override;
	static BlankRectangle* fromXml(const XmlElement& element);

private:
	JUCE_LEAK_DETECTOR(BlankRectangle)
};