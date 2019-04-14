#include "woap_configuration_editor.h"

using namespace WOAP;
using namespace WOAP::GUI;

ConfigurationEditor::ConfigurationEditor()
{
	propertyPanel = new PropertyPanel();
	addAndMakeVisible(propertyPanel);

	saveButton = new TextButton("Save");
	saveButton->addListener(this);
	saveButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(saveButton);

	resetButton = new TextButton("Undo");
	resetButton->addListener(this);
	resetButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(resetButton);

	ConfigurationManager::getInstance()->addListener(this);
	fillPanel();
}

ConfigurationEditor::~ConfigurationEditor()
{
	ConfigurationManager::getInstance()->removeListener(this);
}

void ConfigurationEditor::fillPanel()
{
	propertyPanel->clear();
	config = new ConfigurationManager::Configuration(ConfigurationManager::getInstance()->getConfiguration());

	Array<PropertyComponent*> props({

		new BooleanPropertyComponent(config->loadPreviousSession, "Load previous session", "Enabled"),
		new TextPropertyComponent(config->sessionStoreIntervalSecs, "Session store interval (s)", 0, false),

		new BooleanPropertyComponent(config->anonymousLoginEnabled, "Anonymous login enabled", "Enabled"),
		new TextPropertyComponent(config->adminPassword, "Administrator password", 0, false),

		new BooleanPropertyComponent(config->WLANEnabled, "WLAN enabled", "Enabled"),
		new TextPropertyComponent(config->WLANName, "WLAN name", 0, false),
		new TextPropertyComponent(config->WLANPassword, "WLAN password", 0, false)
	});

	for (PropertyComponent* prop : props)
		prop->setPreferredHeight(30);

	propertyPanel->addProperties(props);
}

void ConfigurationEditor::resized()
{
	propertyPanel->setBounds(getLocalBounds().withTrimmedBottom(40));

	saveButton->setBounds(0, getHeight() - 35, 80, 30);
	resetButton->setBounds(90, getHeight() - 35, 80, 30);
}

void ConfigurationEditor::saveConfiguration()
{
	String validationErrorMsg;

	if (!ConfigurationManager::getInstance()->setConfiguration(*config, validationErrorMsg))
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"Configuration is invalid: " + validationErrorMsg + ".",
			String(),
			nullptr
		);
		return;
	}
	
	if (ConfigurationManager::getInstance()->saveConfig())
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::InfoIcon,
			"Success",
			"Configuration saved sucessfully. Some chanes may need to restart the platform.",
			String(),
			nullptr
		);
	}
	else
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"Something went wrong when saving a file.",
			String(),
			nullptr
		);
	}
}

void ConfigurationEditor::buttonClicked(Button* b)
{
	if (b == resetButton)
	{
		fillPanel();
	}
	else if (b == saveButton)
	{
		saveConfiguration();
	}
}

void ConfigurationEditor::configurationChanged(const ConfigurationManager::Configuration&, const ConfigurationManager::Configuration&)
{
	fillPanel();
}
