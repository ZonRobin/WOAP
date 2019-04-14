#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter_control.h"

class ParameterControlButton : public ParameterControl
{
public:
	ParameterControlButton(PluginParameter* parameter);
	ParameterControlButton(const ParameterControl& other);
	virtual ~ParameterControlButton();

	virtual void paint(Graphics& g) override;
	virtual void resized() override;

	virtual void parameterChanged(PluginParameter*) override;
	virtual void colourSchemeChanged() override;
	virtual void invertedChanged() override;

	String getButtonText() const { return buttonLabel->getText(); }

	virtual XmlElement* createXml() const override;
	static ParameterControlButton* fromXml(const XmlElement& element);

protected:
	ScopedPointer<TextButton> button;
	ScopedPointer<Label> buttonLabel;

private:
	JUCE_LEAK_DETECTOR(ParameterControlButton)
};