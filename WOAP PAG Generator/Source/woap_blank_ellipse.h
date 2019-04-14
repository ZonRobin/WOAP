#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_view_editor_element.h"
#include "woap_colour_scheme.h"

class BlankEllipse : public ViewEditorElement
{
public:
	BlankEllipse();
	BlankEllipse(const BlankEllipse& other);
	~BlankEllipse() {}

	virtual void paint(Graphics& g) override;

	virtual XmlElement* createXml() const override;
	static BlankEllipse* fromXml(const XmlElement& element);

private:
	JUCE_LEAK_DETECTOR(BlankEllipse)
};