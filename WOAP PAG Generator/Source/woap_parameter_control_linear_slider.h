#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter_control.h"

class ParameterControlLinearSlider : public ParameterControl
{
public:
	enum Orientation
	{
		Horizontal,
		Vertical
	};

	ParameterControlLinearSlider(Orientation o, PluginParameter* parameter);
	ParameterControlLinearSlider(const ParameterControl& other);
	virtual ~ParameterControlLinearSlider();

	virtual void paint(Graphics& g) override;

	bool isHorizontal() { return orientation == Orientation::Horizontal; }
	bool isVertical() { return orientation == Orientation::Vertical; }

	void showValueBar(bool show)
	{
		valueBarShown = show;
		repaint();
	}

	virtual XmlElement* createXml() const override;
	static ParameterControlLinearSlider* fromXml(const XmlElement& element);

protected:
	bool valueBarShown = true;
	Orientation orientation;

private:
	JUCE_LEAK_DETECTOR(ParameterControlLinearSlider)
};