#include "woap_main_component.h"
#include "woap_main.h"
#include "woap_view_toolbar.h"
#include "woap_plugin_parameter.h"
#include "woap_entity_manager.h"
#include "woap_view_editor.h"
#include "woap_loading_screen_component.h"
#include "woap_functions.h"
#include "woap_help_box.h"
#include "woap_component_visibilitor.h"
#include "woap_exceptions.h"
#include "woap_look_and_feel.h"

const int MainComponent::MIN_HEIGHT = 670;
const int MainComponent::MIN_WIDTH = 500;

MainComponent::MainComponent()
{
	EntityManager::setMainComponent(this);

	menuBar = new MenuBarComponent(this);
	addAndMakeVisible(menuBar);

	Main::getApplicationCommandManager().registerAllCommandsForTarget(this);
	Main::getApplicationCommandManager().registerAllCommandsForTarget(JUCEApplication::getInstance());

	viewEditor = new ViewEditor();
	addAndMakeVisible(viewEditor);
	
	viewToolbar = new ViewToolbar();
	addAndMakeVisible(viewToolbar);

	loadingScreen = new LoadingScreenComponent();
	loadingScreen->setAlpha(0.8f);
	loadingScreen->setAlwaysOnTop(true);
	addChildComponent(loadingScreen);

	setSize(MainComponent::MIN_WIDTH, MainComponent::MIN_HEIGHT);
}

MainComponent::~MainComponent()
{
	pluginEditor = nullptr;
	shutdownAudio();
}

void MainComponent::addListener(MainComponent::Listener* listener)
{
	listeners.add(listener);
}

void MainComponent::removeListener(MainComponent::Listener* listener)
{
	listeners.remove(listener);
}

int MainComponent::getViewToolbarWidth() const
{
	return 300;
}

void MainComponent::prepareToPlay(int, double)
{

}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{

}

void MainComponent::paint(Graphics& g)
{
	Rectangle<int> bounds = getLocalBounds();

	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
	const Colour highlight = WOAPLookAndFeel::getHeaderBackgroundColour();

	g.setColour(highlight);
	g.drawLine(
		(float)bounds.getWidth() - getViewToolbarWidth(), 
		(float)LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight(), 
		(float)bounds.getWidth() - getViewToolbarWidth(), 
		(float)bounds.getHeight()
	);
}

void MainComponent::resized()
{
	Rectangle<int> bounds = getLocalBounds();

	loadingScreen->setBounds(bounds);
	menuBar->setBounds(bounds.removeFromTop(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));

	Rectangle<int> editorBounds;
	const Image& snapshot = viewEditor->getSnapshot();
	if (snapshot.isNull())
	{
		editorBounds = bounds.withTrimmedRight(getViewToolbarWidth());
	}
	else
	{
		editorBounds = bounds
			.withSizeKeepingCentre(snapshot.getWidth(), snapshot.getHeight())
			.withX(0);
	}
	viewEditor->setBounds(editorBounds);

	const Rectangle<int> rightBar = bounds.withTrimmedLeft(bounds.getWidth() - getViewToolbarWidth());
	const Rectangle<int> toolbarBounds = rightBar.withHeight(545);
	viewToolbar->setBounds(toolbarBounds);

	if (helpBox != nullptr)
	{
		const Rectangle<int> helpBoxBounds = rightBar.withTrimmedTop(545);
		helpBox->setBounds(helpBoxBounds);
	}
}

void MainComponent::resizeToFitSnapshot(Image& snapshot)
{
	const int newWidth = jmax(MainComponent::MIN_WIDTH, snapshot.getWidth() + getViewToolbarWidth());
	const int newHeight = jmax(MainComponent::MIN_HEIGHT, snapshot.getHeight() + LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight());

	getParentComponent()->centreWithSize(newWidth, newHeight);
}

StringArray MainComponent::getMenuBarNames()
{
	return { "File", "Help" };
}

PopupMenu MainComponent::getMenuForIndex(int topLevelMenuIndex, const String&)
{
	ApplicationCommandManager* commandManager = &Main::getApplicationCommandManager();

	PopupMenu menu;

	if (topLevelMenuIndex == 0)
	{
		menu.addCommandItem(commandManager, MainComponent::Command::OpenPluginFile);
		menu.addCommandItem(commandManager, MainComponent::Command::OpenConfigFile);
		menu.addCommandItem(commandManager, MainComponent::Command::ExportAs);
		menu.addSeparator();
		menu.addCommandItem(commandManager, StandardApplicationCommandIDs::quit);
	}
	else if (topLevelMenuIndex == 1)
	{
		menu.addCommandItem(commandManager, MainComponent::Command::StartTutorial);
		menu.addCommandItem(commandManager, MainComponent::Command::StopTutorial);
	}
	
	return menu;
}

void MainComponent::menuItemSelected(int, int)
{

}

ApplicationCommandTarget* MainComponent::getNextCommandTarget()
{
	return nullptr;
}

void MainComponent::getAllCommands(Array<CommandID>& commands)
{
	commands.addArray({
		MainComponent::Command::OpenPluginFile,
		MainComponent::Command::ExportAs,
		MainComponent::Command::OpenPluginEditor,
		MainComponent::Command::OpenConfigFile,
		MainComponent::Command::UnloadPlugin,
		MainComponent::Command::ClosePluginEditor,
		MainComponent::Command::CreatePluginSnapshot,
		MainComponent::Command::StartTutorial,
		MainComponent::Command::StopTutorial
	});
}

void MainComponent::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID) {
	case MainComponent::Command::OpenPluginFile:
		result.setInfo("Open plugin file", "Open plugin file", "file", 0);
		break;
	case MainComponent::Command::OpenConfigFile:
		result.setInfo("Open XML config file", "Open XML config file", "file", 0);
		break;
	case MainComponent::Command::ExportAs:
		result.setInfo("Export XML config file", "Export XML config file", "file", (plugin == nullptr ||  viewEditor->getSnapshot().isNull()) ? ApplicationCommandInfo::isDisabled : 0);
		break;
	case MainComponent::Command::OpenPluginEditor:
		result.setInfo("Open plugin editor", "Open plugin editor", "plugins", 0);
		break;
	case MainComponent::Command::UnloadPlugin:
		result.setInfo("Unload plugin", "Unload plugin", "plugins", 0);
		break;
	case MainComponent::Command::ClosePluginEditor:
		result.setInfo("Close plugin editor", "Close plugin editor", "plugins", 0);
		break;
	case MainComponent::Command::CreatePluginSnapshot:
		result.setInfo("Create plugin snapshot", "Create plugin snapshot", "plugins", 0);
		break;
	case MainComponent::Command::StartTutorial:
		result.setInfo("Start tutorial", "Start tutorial", "help", isHelpBoxOpened() ? ApplicationCommandInfo::isDisabled : 0);
		break;
	case MainComponent::Command::StopTutorial:
		result.setInfo("Stop tutorial", "Stop tutorial", "help", !isHelpBoxOpened() ? ApplicationCommandInfo::isDisabled : 0);
		break;
	default:
		break;
	}
}

bool MainComponent::perform(const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case MainComponent::Command::OpenPluginFile:
	{
		String path = showPluginFileChooser();
		if (path.isNotEmpty()) 
			openPluginByPath(path);
		return true;
	}
	case MainComponent::Command::OpenConfigFile:
			showConfigFileChooser();
			return true;
	case MainComponent::Command::ExportAs:
		showExportAsFileChooser();
		return true;
	case MainComponent::Command::OpenPluginEditor:
		if (!isPluginLoaded())
		{
			return false;
		}
		openPluginEditor();
		return true;
	case MainComponent::Command::UnloadPlugin:
		if (!isPluginLoaded())
		{
			return false;
		}
		unloadPlugin();
		return true;
	case MainComponent::Command::ClosePluginEditor:
		if (!isPluginEditorOpened())
		{
			return false;
		}
		closePluginEditor();
		return true;
	case MainComponent::Command::CreatePluginSnapshot:
		if (!isPluginEditorOpened())
		{
			return false;
		}
		createPluginSnapshot();
		return true;
	case MainComponent::Command::StartTutorial:
		if (helpBox != nullptr) return false;

		helpBox = new HelpBox();
		addAndMakeVisible(helpBox);
		resized();
		Main::getApplicationCommandManager().commandStatusChanged();
		return true;
	case MainComponent::Command::StopTutorial:
		removeChildComponent(helpBox);
		helpBox = nullptr;
		Main::getApplicationCommandManager().commandStatusChanged();
		return true;
	default:
		return false;
	}
}

String MainComponent::showPluginFileChooser() const
{
	FileChooser pluginChooser("Select a VST plugin to open", File::getSpecialLocation(File::userDesktopDirectory), "*.dll");

	if (pluginChooser.browseForFileToOpen())
	{
		File fileChosen(pluginChooser.getResult());
		return fileChosen.getFullPathName();
	}

	return String();
}

void MainComponent::showExportAsFileChooser()
{
	FileChooser saveFileChooser(
		"Select a file to export an XML file", 
		File::getSpecialLocation(File::userDesktopDirectory),
		"*.xml");

	if (saveFileChooser.browseForFileToSave(true))
	{
		const File fileChoosen = saveFileChooser.getResult();
		
		exportAsXmlTo(fileChoosen.getFullPathName());

		AlertWindow::showMessageBox(
			AlertWindow::AlertIconType::InfoIcon,
			"Export sucessful", "The file " + fileChoosen.getFileName() + " was saved sucessfully.");
	}
}

void MainComponent::showConfigFileChooser()
{
	FileChooser pluginChooser("Select a XML config file to open", File::getSpecialLocation(File::userDesktopDirectory), "*.xml");

	if (pluginChooser.browseForFileToOpen())
	{
		File fileChosen(pluginChooser.getResult());
		openConfigByPath(fileChosen.getFullPathName());
	}
}

MainComponent::ConfigLoadingResult MainComponent::openConfigByPath(const String& path)
{
	ComponentVisibilitor visibilitor(loadingScreen);

	File xmlFile(path);
	XmlDocument xmlDoc(xmlFile);
	ScopedPointer<XmlElement> rootElement = xmlDoc.getDocumentElement();

	if (rootElement == nullptr || !rootElement->hasTagName("PAGConfig") || rootElement->getStringAttribute("version") != Main::PAGVersion)
	{
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "The config you selected is not compatible with this app.");
		return ConfigLoadingResult::WrongConfigVersion;
	}
	
	if (rootElement->getNumChildElements() != 1 || !rootElement->getFirstChildElement()->hasTagName("plugin"))
	{
		if (rootElement->getNumChildElements() > 1)
		{
			AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "There was " + String(rootElement->getNumChildElements()) + " plugin sections found in the config.");
			return ConfigLoadingResult::MultiplePluginsFounds;
		}
		else
		{
			AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "There was no plugin section found in the config.");
			return ConfigLoadingResult::NoPluginsFound;
		}
	}

	XmlElement* pluginElement = rootElement->getFirstChildElement();

	if (isPluginLoaded())
	{
		try {
			loadFromXml(pluginElement);
			return ConfigLoadingResult::Ok;
		}
		catch (const UnmatchingConfigException& e) {
			AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "This plugin doesn't match the config file: " + e.getMessage());
		}
		catch (const ParsingException& e) {
			AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "There is an error in the config file: " + e.getMessage());
			return ConfigLoadingResult::ConfigError;
		}
	}
	
	while (true)
	{
		String pluginPath = showPluginFileChooser();
		if (pluginPath.isEmpty())
		{
			return ConfigLoadingResult::Ok;
		}

		if (openPluginByPath(pluginPath, false, false) == PluginLoadingResult::Ok)
		{
			try {
				loadFromXml(pluginElement);
				return ConfigLoadingResult::Ok;
			}
			catch (const UnmatchingConfigException& e) {
				AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "This plugin doesn't match the config file: " + e.getMessage());
			}
			catch (const ParsingException& e) {
				AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "There is an error in the config file: " + e.getMessage());
				return ConfigLoadingResult::ConfigError;
			}
		}
	}

	return ConfigLoadingResult::Ok;
}

bool MainComponent::isInterestedInFileDrag(const StringArray& files)
{
	return files.size() == 1 && (files[0].endsWith(".dll") || files[0].endsWith(".xml"));
}

void MainComponent::filesDropped(const StringArray& files, int, int)
{
	if (files[0].endsWith(".dll"))
		openPluginByPath(files[0]);
	else if (files[0].endsWith(".xml"))
		openConfigByPath(files[0]);
}

MainComponent::PluginLoadingResult MainComponent::openPluginByPath(const String& path, bool openEditor, bool createSnapshot)
{
	ComponentVisibilitor visibilitor(loadingScreen);

	AudioPluginFormatManager formatManager;
	formatManager.addDefaultFormats();

	OwnedArray<PluginDescription> pluginTypes;
	KnownPluginList pluginList;
	pluginList.scanAndAddDragAndDroppedFiles(formatManager, path, pluginTypes);

	if (pluginTypes.size() == 0)
	{
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "No plugin found in " + path);
		return MainComponent::PluginLoadingResult::NoPluginRecognized;
	}

	if (pluginTypes.size() > 1)
	{
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "More than one plugin found in " + path);
		return MainComponent::PluginLoadingResult::ManyPluginsRecognized;
	}

	unloadPlugin();

	String error;
	plugin = formatManager.createPluginInstance(*pluginTypes.getFirst(), 44100, 512, error);
	if (plugin == nullptr)
	{
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error", "Can\'t create plugin instance. Reason: " + error);
		return MainComponent::PluginLoadingResult::PluginCreationFailed;
	}

	pluginDescription = pluginTypes.getFirst();
	pluginTypes.clearQuick(false);

	pluginChanged();

	if (openEditor)
	{
		openPluginEditor();

		if (createSnapshot)
		{
			visibilitor.setActive(false);
			Timer::callAfterDelay(700, [this]() {
				ComponentVisibilitor visibilitor(loadingScreen);
				createPluginSnapshot();
			});
		}
	}
	
	return MainComponent::PluginLoadingResult::Ok;
}

void MainComponent::unloadPlugin()
{
	closePluginEditor();

	listeners.call(&MainComponent::Listener::pluginWillBeDeleted);
	pluginParameters.clearQuick(true);
	pluginDescription = nullptr;
	plugin = nullptr;
	Main::getApplicationCommandManager().commandStatusChanged();
}

bool MainComponent::isPluginLoaded() const
{
	return plugin != nullptr;
}

AudioProcessor* MainComponent::getPlugin() const
{
	return plugin;
}

PluginParameter* MainComponent::getPluginParameter(const String& ref)
{
	for (PluginParameter* parameter : pluginParameters)
	{
		if (parameter->getRef() == ref)
			return parameter;
	}

	return nullptr;
}

void MainComponent::pluginChanged()
{
	const int numParameters = plugin->getNumParameters();
	for (int i = 0; i < numParameters; ++i)
	{
		PluginParameter* parameter = new PluginParameter(i, plugin->getParameterName(i));
		parameter->setValueAndText(plugin->getParameter(i), plugin->getParameterText(i));
		pluginParameters.add(parameter);
	}

	plugin->addListener(this);

	listeners.call(&MainComponent::Listener::pluginLoaded, plugin, pluginDescription);
	Main::getApplicationCommandManager().commandStatusChanged();
}

AudioProcessorEditor* MainComponent::openPluginEditor()
{
	AudioProcessorEditor* editor = plugin->createEditorIfNeeded();

	if (pluginEditor != nullptr) return editor;

	pluginEditor = new Window(this, plugin->getName(), Colours::black, false);
	pluginEditor->setTitleBarButtonsRequired(DocumentWindow::TitleBarButtons::closeButton, false);
	pluginEditor->setContentOwned(editor, true);
	pluginEditor->setDraggable(true);
	pluginEditor->setVisible(true);
	pluginEditor->setCentreRelative(0.5f, 0.5f);

	listeners.call(&MainComponent::Listener::pluginEditorCreated, editor);
	return editor;
}

void MainComponent::closePluginEditor()
{
	listeners.call(&MainComponent::Listener::pluginEditorWillBeDeleted);
	pluginEditor = nullptr;
}

AudioProcessorEditor* MainComponent::getPluginEditor()
{
	if (!isPluginEditorOpened())
	{
		return nullptr;
	}

	return plugin->createEditorIfNeeded();
}

bool MainComponent::isPluginEditorOpened() const
{
	return pluginEditor != nullptr;
}

void MainComponent::closeWindowTriggered(Window* closedWindow)
{
	if (closedWindow == pluginEditor)
	{
		closePluginEditor();
	}
}

void MainComponent::createPluginSnapshot()
{
	Image snapshot(createSnapshotOfNativeWindow(pluginEditor->getContentComponent()->getWindowHandle()));
	snapshot = snapshot.getClippedImage(snapshot.getBounds().withTrimmedTop(LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight()));
	setPluginSnapshot(snapshot);
}

void MainComponent::setPluginSnapshot(Image& snapshot)
{
	viewEditor->setPluginSnapshot(snapshot);
	resizeToFitSnapshot(snapshot);
	Main::getApplicationCommandManager().commandStatusChanged();
}

void MainComponent::setParameterValue(int index, float newValue)
{
	plugin->setParameterNotifyingHost(index, newValue);
}

void MainComponent::parameterTouched(int parameterIndex)
{
	activeParameter = pluginParameters.getUnchecked(parameterIndex);
	listeners.call(&MainComponent::Listener::pluginParameterTouched, activeParameter);
}

void MainComponent::showLoadingScreen()
{
	loadingScreen->setVisible(true);
}

void MainComponent::hideLoadingScreen()
{
	loadingScreen->setVisible(false);
}

void MainComponent::audioProcessorParameterChanged(AudioProcessor*, int parameterIndex, float newValue)
{
	PluginParameter* parameter = pluginParameters.getUnchecked(parameterIndex);
	String newValueText = plugin->getParameterText(parameterIndex);

	parameter->setValueAndText(newValue, newValueText);

	if (activeParameter != parameter)
	{
		activeParameter = parameter;
		listeners.call(&MainComponent::Listener::pluginParameterTouched, parameter);
	}
}

void MainComponent::audioProcessorChanged(AudioProcessor*)
{

}

void MainComponent::exportAsXmlTo(const String& filePathToExportTo) const
{
	File fileToExportTo(filePathToExportTo);
	ScopedPointer<XmlElement> rootElement = new XmlElement("PAGConfig");
	rootElement->setAttribute("version", Main::PAGVersion);
	
	XmlElement* pluginElement = new XmlElement("plugin");
	pluginElement->setAttribute("ref", pluginDescription->name);
	pluginElement->setAttribute("manufacturer", pluginDescription->manufacturerName);
	pluginElement->setAttribute("version", pluginDescription->version);
	pluginElement->setAttribute("id", pluginDescription->uid);
	rootElement->addChildElement(pluginElement);

	XmlElement* parametersElement = new XmlElement("parameters");
	pluginElement->addChildElement(parametersElement);

	for (PluginParameter* parameter : pluginParameters)
	{
		parametersElement->addChildElement(parameter->createXml());
	}

	XmlElement* viewsElement = new XmlElement("views");
	pluginElement->addChildElement(viewsElement);
	viewsElement->addChildElement(viewEditor->createXml());

	XmlElement* imagesElement = new XmlElement("images");
	pluginElement->addChildElement(imagesElement);

	ScopedPointer<HashMap<String, const Image*>> images = viewEditor->getImagesList();
	for (HashMap<String, const Image*>::Iterator i(*images); i.next();)
	{
		XmlElement* imageElement = new XmlElement("image");
		imagesElement->addChildElement(imageElement);

		imageElement->setAttribute("ref", i.getKey());

		ScopedPointer<MemoryOutputStream> encoderStream = imageToBase64(*i.getValue());
		imageElement->addTextElement(encoderStream->toString());
	}

	rootElement->writeToFile(fileToExportTo, "", "UTF-8", 1000);
}

void MainComponent::loadFromXml(const XmlElement* pluginElement)
{
	listeners.call(&MainComponent::Listener::pluginWillBeDeleted);

	const String pluginRef = pluginElement->getStringAttribute("ref");
	const String pluginManufacturer = pluginElement->getStringAttribute("manufacturer");
	const String pluginVersion = pluginElement->getStringAttribute("version");
	const int pluginId = pluginElement->getIntAttribute("id");
	
	if (pluginRef.isEmpty()) throw ParsingException("Plugin reference is empty");
	if (pluginManufacturer.isEmpty()) throw ParsingException("Plugin manufacturer is empty");
	if (pluginVersion.isEmpty()) throw ParsingException("Plugin version is empty");
	if (pluginId == 0) throw ParsingException("Plugin id is empty");

	const XmlElement* parametersElement = pluginElement->getChildByName("parameters");
	if (parametersElement == nullptr) throw ParsingException("Parameters element not found");

	if (pluginDescription != nullptr && plugin != nullptr)
	{
		if (pluginId != pluginDescription->uid) 
			throw UnmatchingConfigException("Config plugin ID " + String(pluginId) + " doesn't match loaded plugin id " + String(pluginDescription->uid));

		if (pluginManufacturer != pluginDescription->manufacturerName)
			throw UnmatchingConfigException("Config plugin manufacturer " + String(pluginManufacturer) + " doesn't match loaded plugin manufacturer " + String(pluginDescription->manufacturerName));

		if (pluginVersion != pluginDescription->version)
			throw UnmatchingConfigException("Config plugin version " + String(pluginVersion) + " doesn't match loaded plugin version " + String(pluginDescription->version));

		if (pluginRef != pluginDescription->name)
			throw UnmatchingConfigException("Config plugin name " + String(pluginRef) + " doesn't match loaded plugin name " + String(pluginDescription->name));

		if (parametersElement->getNumChildElements() != plugin->getNumParameters())
			throw UnmatchingConfigException("Config plugin number of parameters " + String(parametersElement->getNumChildElements()) + " doesn't match loaded plugin id " + String(plugin->getNumParameters()));
	}
	else
	{
		pluginDescription = new PluginDescription();
		pluginDescription->uid = pluginId;
		pluginDescription->manufacturerName = pluginManufacturer;
		pluginDescription->version = pluginVersion;
		pluginDescription->name = pluginRef;
		plugin = nullptr;
	}

	pluginParameters.clearQuick(true);
	activeParameter = nullptr;

	int parameterIndex = 0;
	forEachXmlChildElement(*parametersElement, parameterElement)
	{
		PluginParameter* param = PluginParameter::fromXml(parameterIndex, *parameterElement);
		pluginParameters.add(param);
		param->setValueAndText(plugin->getParameter(parameterIndex), plugin->getParameterText(parameterIndex));
		++parameterIndex;
	}

	listeners.call(&MainComponent::Listener::pluginLoaded, plugin, pluginDescription);
	
	const XmlElement* imagesElement = pluginElement->getChildByName("images");
	if (imagesElement == nullptr) throw ParsingException("Images element not found");
	forEachXmlChildElement(*imagesElement, imageElement)
	{
		if (imageElement->getStringAttribute("ref") != "background")
		{
			throw ParsingException("Found unknown image. Ref: " + imageElement->getStringAttribute("ref"));
		}

		Image background = base64ToImage(imageElement->getAllSubText());
		if (background.isValid())
		{
			setPluginSnapshot(background);
		}
	}

	const XmlElement* viewsElement = pluginElement->getChildByName("views");
	if (viewsElement == nullptr) throw ParsingException("Views element not found");
	if (viewsElement->getNumChildElements() == 0) throw ParsingException("No view found");
	if (viewsElement->getNumChildElements() != 1) throw ParsingException("Only one view supported");
	viewEditor->loadFromXml(*viewsElement->getFirstChildElement());
}