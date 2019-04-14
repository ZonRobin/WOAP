#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_main_component.h"
#include "woap_plugin_parameter.h"
#include "woap_colour_input.h"
#include "woap_parameter_control.h"
#include "woap_view_editor.h"

class PluginParameter;
class ParameterControlButton;
class ParameterControlLinearSlider;
class ParameterControlRotarySlider;

class PluginParameterToolbar : 
	public Component, 
	public MainComponent::Listener, 
	public PluginParameter::Listener, 
	public ColourInput::Listener, 
	public ParameterControl::ValueListener, 
	public Button::Listener, 
	public ViewEditor::Listener
{
public:
	PluginParameterToolbar();
	~PluginParameterToolbar();

	virtual void paint(Graphics& g) override;	

	virtual void pluginWillBeDeleted() override;
	virtual void pluginLoaded(AudioProcessor* processor, PluginDescription* description) override;
	virtual void pluginEditorWillBeDeleted() override;
	virtual void pluginEditorCreated(AudioProcessorEditor* editor) override;
	virtual void pluginParameterTouched(PluginParameter* pluginParameter) override;
	virtual void numStepsChanged(int newNumSteps) override;
	virtual void typeChanged(PluginParameter::Type newType) override;

	virtual void parameterControlCreated(ParameterControl*) override;
	virtual void parameterControlValueChanged(ParameterControl* control, float newValue) override;
	virtual void parameterControlWillDelete(ParameterControl*) override;
	virtual void parameterChanged(PluginParameter* parameter) override;

	virtual void colourSelected(ColourInput* input, const Colour& colour) override;
	virtual void buttonClicked(Button*) override;

	void colourChanged();
	void invertedChanged();
	void activeControlChanged();

	void removeParameterControls();

protected:
	ScopedPointer<ParameterControlRotarySlider> rotarySlider;
	ScopedPointer<ParameterControlLinearSlider> verticalSlider;
	ScopedPointer<ParameterControlLinearSlider> verticalNoBarSlider;
	ScopedPointer<ParameterControlLinearSlider> horizontalSlider;
	ScopedPointer<ParameterControlLinearSlider> horizontalNoBarSlider;
	ScopedPointer<ParameterControlButton> buttonControl;
	ScopedPointer<ToggleButton> inversionButton;
	PluginParameter* activeParameter = nullptr;
	ParameterControl* activeControl = nullptr;

	ScopedPointer<ColourInput> colourInputFill;
	ScopedPointer<ColourInput> colourInputAccent;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameterToolbar)
};