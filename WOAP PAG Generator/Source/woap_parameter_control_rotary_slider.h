#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter_control.h"

class ParameterControlRotarySlider : public ParameterControl
{
public:
	ParameterControlRotarySlider(PluginParameter* parameter);
	ParameterControlRotarySlider(const ParameterControl& other);
	virtual ~ParameterControlRotarySlider();

	virtual void paint(Graphics& g) override;

	virtual XmlElement* createXml() const override;
	static ParameterControlRotarySlider* fromXml(const XmlElement& element);

private:
	JUCE_LEAK_DETECTOR(ParameterControlRotarySlider)
};