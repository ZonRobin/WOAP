#include "woap_view_toolbar.h"
#include "woap_main.h"
#include "woap_main_component.h"
#include "woap_parameter_control.h"
#include "woap_entity_manager.h"
#include "woap_plugin_parameter_toolbar.h"
#include "woap_elements_toolbar.h"
#include "woap_look_and_feel.h"

ViewToolbar::ViewToolbar()
{
	createPluginSnapshotButton = new TextButton("Create plugin snapshot");
	createPluginSnapshotButton->addListener(this);
	addChildComponent(createPluginSnapshotButton);

	openPluginEditorButton = new TextButton("Open plugin editor");
	openPluginEditorButton->addListener(this);
	addChildComponent(openPluginEditorButton);

	MainComponent* mainComponent = EntityManager::getMainComponent();
	mainComponent->addListener(this);
	editorOpened = mainComponent->isPluginEditorOpened();

	parameterToolbar = new PluginParameterToolbar();
	addAndMakeVisible(parameterToolbar);

	elementsToolbar = new ElementsToolbar();
	addChildComponent(elementsToolbar);

	updateLayout();
}

ViewToolbar::~ViewToolbar()
{
	EntityManager::getMainComponent()->removeListener(this);
}

void ViewToolbar::paint(Graphics& g)
{
	if (plugin == nullptr)
		return;

	const Rectangle<int> bounds = getLocalBounds();
	const Colour text = WOAPLookAndFeel::getTextColour();
	const Colour highlight = WOAPLookAndFeel::getHeaderBackgroundColour();
	const Rectangle<int> headingBounds = bounds.withHeight(40);
	const String headingText = String(pluginDescription->name + " v" + pluginDescription->version + "\nby " + pluginDescription->manufacturerName);

	g.setColour(highlight);
	g.fillRect(headingBounds);
	g.setColour(text);
	g.drawFittedText(headingText, headingBounds, Justification::centred, 2);
}

void ViewToolbar::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	createPluginSnapshotButton->setBounds(bounds.getWidth() / 4, 45, bounds.getWidth() / 2, 25);
	openPluginEditorButton->setBounds(bounds.getWidth() / 4, 45, bounds.getWidth() / 2, 25);
	
	bounds.removeFromTop(75);
	elementsToolbar->setBounds(bounds.withHeight(175));

	bounds.removeFromTop(145);
	parameterToolbar->setBounds(bounds);
}

void ViewToolbar::buttonClicked(Button* btn)
{
	if (btn == createPluginSnapshotButton)
	{
		Main::invokeCommand(MainComponent::Command::CreatePluginSnapshot);
	}
	else if (btn == openPluginEditorButton)
	{
		Main::invokeCommand(MainComponent::Command::OpenPluginEditor);
	}
}

void ViewToolbar::pluginWillBeDeleted()
{
	plugin = nullptr;
	pluginDescription = nullptr;
	elementsToolbar->setVisible(false);
	repaint();
}

void ViewToolbar::pluginLoaded(AudioProcessor* processor, PluginDescription* description)
{
	plugin = processor;
	pluginDescription = description;
	elementsToolbar->setVisible(true);
	editorOpened = false;
	updateLayout();
}

void ViewToolbar::pluginEditorWillBeDeleted()
{
	editorOpened = false;
	updateLayout();
}

void ViewToolbar::pluginEditorCreated(AudioProcessorEditor*)
{
	editorOpened = true;
	updateLayout();
}

void ViewToolbar::pluginParameterTouched(PluginParameter*)
{
}

void ViewToolbar::updateLayout()
{
	openPluginEditorButton->setVisible(plugin != nullptr && !editorOpened);
	createPluginSnapshotButton->setVisible(plugin != nullptr && editorOpened);

	repaint();
}