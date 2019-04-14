#include "woap_settings_view.h"
#include "woap_solo_bus_settings_component.h"
#include "woap_configuration_editor.h"
#include "woap_audio_core.h"
#include "woap_main.h"
#include "woap_look_and_feel.h"
#include "woap_loading_spinner.h"

using namespace WOAP;
using namespace WOAP::GUI;

SettingsView::SettingsView()
{
	scanForPluginsButton = new TextButton("Scan for plugins");
	scanForPluginsButton->addListener(this);
	scanForPluginsButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(scanForPluginsButton);

	importPluginsButton = new TextButton("Import plugins");
	importPluginsButton->addListener(this);
	importPluginsButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(importPluginsButton);

	importPagTemplatesButton = new TextButton("Import PAG templates");
	importPagTemplatesButton->addListener(this);
	importPagTemplatesButton->setMouseCursor(MouseCursor::PointingHandCursor);
	addAndMakeVisible(importPagTemplatesButton);

	soloBusSettings = new SoloBusSettingsComponent();
	addAndMakeVisible(soloBusSettings);
	
	configEditor = new ConfigurationEditor();
	addAndMakeVisible(configEditor);

	audioDeviceSelectorLf = WOAPLookAndFeel::creteAudioDeviceSelectorLookAndFeel();

	audioDeviceSelector = new AudioDeviceSelectorComponent(*AudioCore::getInstance()->getAudioDeviceManager(), 0, 1024, 2, 1024, false, false, false, false);
	audioDeviceSelector->setLookAndFeel(audioDeviceSelectorLf);
	addAndMakeVisible(audioDeviceSelector);

	pluginLoadingSpinner = new LoadingSpinner(true);
	addChildComponent(pluginLoadingSpinner);
}

SettingsView::~SettingsView()
{
	PluginManager::getInstance()->removeListener(this);

	audioDeviceSelector = nullptr;
	audioDeviceSelectorLf = nullptr;

	if (pluginImporter != nullptr)
		pluginImporter->stopThread(1000);
}

void SettingsView::resized()
{
	scanForPluginsButton->setBounds(10, 60, 150, 30);
	importPluginsButton->setBounds(170, 60, 150, 30);
	pluginLoadingSpinner->setBounds(90, 22, 26, 26);

	importPagTemplatesButton->setBounds(10, 170, 150, 30);

	soloBusSettings->setBounds(10, 280, 300, 40);
	configEditor->setBounds(10, 380, 370, 255);

	audioDeviceSelector->setBounds(400, 45, 450, 400);
}

void SettingsView::paint(Graphics& g)
{
	g.fillAll(Colours::grey);

	g.setFont(23.0f);
	g.setColour(Colours::black);

	g.drawText("Plugins", 10, 10, 200, 50, Justification::left, false);
	g.drawText("PAG templates", 10, 120, 200, 50, Justification::left, false);
	g.drawText("Solo bus", 10, 230, 200, 50, Justification::left, false);
	g.drawText("Global configuration", 10, 330, 200, 50, Justification::left, false);

	g.drawText("Audio device", 400, 10, 200, 50, Justification::left, false);

	g.setColour(Colours::darkgrey);
	g.drawHorizontalLine(52, 10, 380);
	g.drawHorizontalLine(162, 10, 380);
	g.drawHorizontalLine(272, 10, 380);
	g.drawHorizontalLine(372, 10, 380);

	g.drawHorizontalLine(52, 400, 840);
}

void SettingsView::buttonClicked(Button* button)
{
	if (button == scanForPluginsButton)
	{
		PluginManager::getInstance()->addListener(this);
		PluginManager::getInstance()->scanPluginTypes();
	}
	else if (button == importPluginsButton)
	{
		startPluginImport();
	}
	else if (button == importPagTemplatesButton)
	{
		pagTemplatesImporter = new PAGTemplatesImporter();
		pagTemplatesImporter->startImport();
	}
}

void SettingsView::startPluginImport()
{
	if (pluginImporter == nullptr)
		pluginImporter = new PluginImporter();

	if (!pluginImporter->isThreadRunning())
		pluginImporter->startThread();
}

void SettingsView::pluginTypesScanStarted()
{
	pluginLoadingSpinner->setVisible(true);
}

void  SettingsView::pluginTypesScanned(int newNumPlugins)
{
	PluginManager::getInstance()->removeListener(this);
	pluginLoadingSpinner->setVisible(false);

	String message = "Scan finished.";

	if (newNumPlugins == 0)
		message += " No plugin registered.";
	else if (newNumPlugins == 1)
		message += " 1 plugin registered.";
	else
		message += " " + String(newNumPlugins) + " plugins registered.";

	AlertWindow::showMessageBoxAsync(
		AlertWindow::AlertIconType::InfoIcon,
		"Success",
		message
	);
}

SettingsView::PluginImporter::PluginImporter()
	: Thread("Plugin importer")
{

}

SettingsView::PluginImporter::~PluginImporter()
{
}

void SettingsView::PluginImporter::run()
{
	setState(State::ThreadStarted);	
}

void SettingsView::PluginImporter::startImport()
{
	pluginsToImport.clearQuick();

	MessageManager::callAsync([=]() {
		FileChooser chooser("Please select plugins to import",
			File::getSpecialLocation(File::userDesktopDirectory),
			"*.dll", false, false);

		if (!chooser.browseForMultipleFilesToOpen())
		{
			setState(State::CancelledByUser);
			return;
		}

		pluginsToImport.addArray(chooser.getResults());
		setState(State::PluginsSelected);
	});
}

void SettingsView::PluginImporter::importNextPlugin()
{
	if (pluginsToImport.isEmpty())
	{
		setState(State::NothingToImport);
		return;
	}

	File pluginFile = pluginsToImport.removeAndReturn(0);
	importData = PluginManager::getInstance()->loadPluginByPath(pluginFile.getFullPathName());
	if (importData->returnCode != PluginManager::PluginImportData::ReturnCode::Ok)
	{
		jassertfalse;
		AlertWindow::showMessageBoxAsync(
			AlertWindow::AlertIconType::WarningIcon, 
			"Error",
			"Can't load plugin " + pluginFile.getFullPathName(), 
			String(), 
			nullptr, 
			ModalCallbackFunction::create([this] (int) {
				importData = nullptr;
				setState(State::ImportFailed);
			})
		);
		return;
	}

	pluginEditor = new DocumentWindow(importData->pluginInstance->getName(), Colours::black, DocumentWindow::TitleBarButtons::closeButton);
	pluginEditor->setContentOwned(importData->pluginInstance->createEditor(), true);
	pluginEditor->setDraggable(true);
	pluginEditor->setVisible(true);
	pluginEditor->setCentreRelative(0.5f, 0.5f);

	Timer::callAfterDelay(1500, [this]() {
		setState(State::PluginEditorCreated);
	});
}

void SettingsView::PluginImporter::askUserToImport()
{
	AlertWindow::showYesNoCancelBox(
		AlertWindow::AlertIconType::QuestionIcon,
		"Import plugin?",
		"Do you want to import " + importData->pluginDescription->name + "?",
		String(),
		String(),
		String(),
		nullptr,
		ModalCallbackFunction::create([this](int result) {
			switch (result)
			{
			case 0:
				pluginEditor = nullptr;
				importData = nullptr;
				setState(State::CancelledByUser);
				return;
			case 1:
				setState(State::UserWantsImport);
				return;
			case 2:
				pluginEditor = nullptr;
				importData = nullptr;
				setState(State::UserRejectedImport);
				return;
			default:
				return;
			}
		})
	);
}

void SettingsView::PluginImporter::importPlugin()
{
	if (!PluginManager::getInstance()->importPlugin(*importData->pluginDescription))
	{
		AlertWindow::showMessageBoxAsync(
			AlertWindow::AlertIconType::WarningIcon, 
			"Error",
			"Plugin could not be saved. There\'s a plugin with the same name " + importData->pluginDescription->name + " already in use or the file can\'t be copied.",
			String(),
			nullptr,
			ModalCallbackFunction::create([this](int) {
				pluginEditor = nullptr;
				importData = nullptr;
				setState(State::ImportFailed);
			})
		);
	}
	else
	{
		AlertWindow::showMessageBoxAsync(
			AlertWindow::AlertIconType::InfoIcon, 
			"Success", 
			"Plugin " + importData->pluginDescription->name + " successfully imported.",
			String(),
			nullptr,
			ModalCallbackFunction::create([this](int) {
				pluginEditor = nullptr;
				importData = nullptr;
				setState(State::PluginImported);
			})
		);
	}
}

void SettingsView::PluginImporter::stateChanged(PluginImporter::State)
{
	switch (state)
	{
	case State::ThreadStarted:
		startImport();
		return;
	case State::PluginsSelected:
	case State::ImportFailed:
	case State::UserRejectedImport:
		importNextPlugin();
		return;
	case State::PluginEditorCreated:
		askUserToImport();
		return;
	case State::UserWantsImport:
		importPlugin();
		return;
	case State::PluginImported:
		importNextPlugin();
		return;
	case State::CancelledByUser:
	case State::NothingToImport:
		return;
	default:
		return;
	}
}


void SettingsView::PluginImporter::setState(PluginImporter::State newState)
{
	if (state == newState)
		return;

	State oldState = state;
	state = newState;
	stateChanged(oldState);
}






SettingsView::PAGTemplatesImporter::PAGTemplatesImporter()
{

}

SettingsView::PAGTemplatesImporter::~PAGTemplatesImporter()
{

}

void SettingsView::PAGTemplatesImporter::startImport()
{
	Main::getInstanceCast()->getPAGTemplatesDirectory().createDirectory();

	FileChooser chooser("Please select PAG templates to import",
		File::getSpecialLocation(File::userDesktopDirectory),
		"*.xml", false, false);

	if (!chooser.browseForMultipleFilesToOpen())
		return;

	for (const File& file : chooser.getResults())
	{
		if (!importSingleTemplate(file))
			return;
	}	
}

bool SettingsView::PAGTemplatesImporter::importSingleTemplate(const File& tmpl)
{
	ScopedPointer<XmlElement> xmlData = XmlDocument::parse(tmpl);
	if (xmlData == nullptr)
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"There is an error in " + tmpl.getFullPathName() + " file. Skipping."
		);
		return true;
	}

	const int pluginUid = getPluginUid(xmlData);
	if (pluginUid == 0)
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"Can\'t find plugin uid in " + tmpl.getFullPathName() + " file. Skipping."
		);
		return true;
	}

	const String pluginName = getPluginName(xmlData);
	if (pluginName.isEmpty())
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"Can\'t find plugin ref (name) in " + tmpl.getFullPathName() + " file. Skipping."
		);
		return true;
	}

	const File storedTemplate = Main::getInstanceCast()->getPAGTemplatesDirectory().getChildFile("pag-" + String(pluginUid) + ".xml");
	if (storedTemplate.existsAsFile())
	{
		const int result = AlertWindow::showYesNoCancelBox(
			AlertWindow::AlertIconType::QuestionIcon,
			"Overwrite existing template?",
			"A PAG template for " + pluginName + " already exists. Overwrite?"
		);

		if (result == 0)
			return false;

		if (result == 2)
			return true;
	}

	if (tmpl.copyFileTo(storedTemplate))
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::InfoIcon,
			"Success",
			"Template file for " + pluginName + " imported."
		);
	}
	else
	{
		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::WarningIcon,
			"Error",
			"Unknown error occured during copying of " + tmpl.getFullPathName() + " file."
		);
	}

	return true;
}

int SettingsView::PAGTemplatesImporter::getPluginUid(const XmlElement* data) const
{
	if (!data->hasTagName("PAGConfig"))
		return 0;

	const XmlElement* pluginElement = data->getChildByName("plugin");
	if (pluginElement == nullptr)
		return 0;

	return pluginElement->getIntAttribute("id", 0);
}

String SettingsView::PAGTemplatesImporter::getPluginName(const XmlElement* data) const
{
	if (!data->hasTagName("PAGConfig"))
		return "";

	const XmlElement* pluginElement = data->getChildByName("plugin");
	if (pluginElement == nullptr)
		return "";

	return pluginElement->getStringAttribute("ref", "");
}
