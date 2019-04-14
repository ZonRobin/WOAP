#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_main_component.h"

class ParameterControl;
class PluginParameterToolbar;
class ElementsToolbar;

class ViewToolbar : public Component, public Button::Listener, public MainComponent::Listener
{
public:
	ViewToolbar();
	virtual ~ViewToolbar();

	void paint(Graphics& g) override;
	void resized() override;

	virtual void buttonClicked(Button* btn) override;

	virtual void pluginWillBeDeleted() override;
	virtual void pluginLoaded(AudioProcessor* processor, PluginDescription* description) override;
	virtual void pluginEditorWillBeDeleted() override;
	virtual void pluginEditorCreated(AudioProcessorEditor* editor) override;
	virtual void pluginParameterTouched(PluginParameter* pluginParameter) override;

	void updateLayout();

protected:
	AudioProcessor* plugin = nullptr;
	PluginDescription* pluginDescription = nullptr;
	ScopedPointer<TextButton> createPluginSnapshotButton;
	ScopedPointer<TextButton> openPluginEditorButton;
	ScopedPointer<PluginParameterToolbar> parameterToolbar;
	ScopedPointer<ElementsToolbar> elementsToolbar;

	bool editorOpened = false;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ViewToolbar)
};