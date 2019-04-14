#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_main_component.h"

class ParameterAutomizer;

class HelpBox : public Component, public MainComponent::Listener, public Button::Listener
{
public:
	enum Step
	{
		None,
		LoadPlugin,
		ChooseControlElement,
		ResizeControl,
		ChooseColour,
		AllDone
	};

	HelpBox();
	~HelpBox();

	void resized() override;
	void paint(Graphics& g) override;

	String getMessageForStep(Step st);
	void runTutorial();
	void stepTutorialForward();
	void stepTutorialBack();
	void stopTutorial();
	bool canStepForward() const;
	bool canStepBack() const;
	
	void stepChanged();

	void startParameterAutomation();
	void stopParameterAutomation();

	virtual void pluginWillBeDeleted() override;
	virtual void pluginLoaded(AudioProcessor*, PluginDescription*) override;
	virtual void pluginEditorWillBeDeleted() override;
	virtual void pluginEditorCreated(AudioProcessorEditor*) override;
	virtual void pluginParameterTouched(PluginParameter*) override;

	virtual void buttonClicked(Button*) override;

protected:
	Step step;
	int parameterHandled = -1;
	ScopedPointer<ParameterAutomizer> automizer;
	ScopedPointer<TextButton> stepForwardButton;
	ScopedPointer<TextButton> stepBackButton;
	ScopedPointer<Label> hintLabel;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HelpBox)
};