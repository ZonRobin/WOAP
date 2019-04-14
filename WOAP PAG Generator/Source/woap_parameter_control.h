#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_plugin_parameter.h"
#include "woap_view_editor_node.h"
#include "woap_colour_scheme.h"

class ParameterControl : public ViewEditorNode, public PluginParameter::Listener
{
public:
	class ValueListener
	{
	public:
		virtual void parameterControlValueChanged(ParameterControl* control, float newValue) = 0;
	};

	ParameterControl(PluginParameter* parameter);
	ParameterControl(const ParameterControl& other);
	virtual ~ParameterControl();

	PluginParameter* getPluginParameter() const { return pluginParameter; }

	virtual void mouseWheelMove(const MouseEvent&, const MouseWheelDetails&) override;

	void addValueListener(ValueListener* listenerToAdd) { valueListeners.add(listenerToAdd); }
	void removeValueListener(ValueListener* listenerToRemove) { valueListeners.remove(listenerToRemove); }

	float getValue() const { return pluginParameter->getValue(); }

	void setInverted(bool newInverted) { inverted = newInverted; invertedChanged(); }
	bool isInverted() const { return inverted; }

	void setColourScheme(ColourScheme newScheme) { colourScheme = newScheme; colourSchemeChanged(); }
	ColourScheme getColourScheme() { return colourScheme; }

	virtual void colourSchemeChanged() { repaint(); }
	virtual void invertedChanged() { repaint(); }

	virtual void parameterChanged(PluginParameter*) override { repaint(); }

	void drawOutlineIfFocused(Graphics& g);

protected:
	PluginParameter* pluginParameter;
	ColourScheme colourScheme;
	bool inverted = false;
	ListenerList<ValueListener> valueListeners;
	Time lastMouseWheelTime;

	void mouseWheelUp();
	void mouseWheelDown();

private:
	JUCE_LEAK_DETECTOR(ParameterControl)
};