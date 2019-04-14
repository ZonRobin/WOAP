#include "woap_main.h"
#include "woap_main_view.h"
#include "woap_audio_core.h"
#include "woap_api_core.h"
#include "woap_bootstrapper.h"
#include "woap_state_manager.h"
#include "woap_look_and_feel.h"
#include "woap_configuration_manager.h"
#include "woap_access_control_core.h"
#include "woap_plugin_manager.h"
#include "woap_functions.h"

using namespace WOAP;
using namespace WOAP::GUI;

Main::Main() 
	: deviceVersion("1.0"),
	deviceId("4da8cb1b5a804334b8ddae8ccb319c4c"),
	kioskMode(false),
	kioskModeToggle(true),
	useGUI(true)
{
	commandManager = new ApplicationCommandManager();
	commandManager->registerAllCommandsForTarget(this);

	Process::setPriority(Process::HighPriority);
}

Main::~Main()
{
	
}

File Main::getApplicationDirectory() const
{
	return File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory();
}

File Main::getHostApplicationDirectory() const
{
	return getApplicationDirectory();
}

File Main::getPluginsDirectory() const
{
	return getHostApplicationDirectory().getChildFile("Plugins");
}

File Main::getPAGTemplatesDirectory() const
{
	return getApplicationDirectory().getChildFile("WebClient").getChildFile("pag-templates");
}

File Main::getApplicationStateFile() const
{
	return getHostApplicationDirectory().getChildFile("state.xml");
}

File Main::getApplicationConfigFile() const
{
	return getHostApplicationDirectory().getChildFile("config.xml");
}

File Main::getAudioDeviceSettingsFile() const
{
	return getHostApplicationDirectory().getChildFile("audioConfig.xml");
}

File Main::getPluginBlacklistFile() const
{
	return getHostApplicationDirectory().getChildFile("pluginBlacklist.txt");
}

File Main::getPluginListFile() const
{
	return getHostApplicationDirectory().getChildFile("pluginList.xml");
}

bool Main::moreThanOneInstanceAllowed()       
{
	return false; 
}

void Main::initialise (const String& /*commandLine*/)
{
	initialised = true;

	getHostApplicationDirectory().createDirectory();

	ConfigurationManager::getInstance()->loadConfig();

	startWLANIfAllowed();

	Bootstrapper::getInstance()->callInitializers();

	LookAndFeel::setDefaultLookAndFeel(new WOAPLookAndFeel());
		
	PluginManager::getInstance();

	AudioCore::getInstance();

	if (isUsingGUI())
	{
		initialiseGui();
		Timer::callAfterDelay(200, [this]() {
			loadApp(); 
		});
	}
	else
	{
		loadApp();
	}
}

void Main::initialiseGui()
{
	mainWindow = new MainWindow(MainView::createInstanceIfNeeded(), getApplicationName(), kioskModeToggle, kioskMode);
}

void Main::loadApp()
{
	ConfigurationManager* config = ConfigurationManager::getInstance();
	AudioCore* core = AudioCore::getInstance();

	core->selectAudioDevice();

	const bool didLoadFromSave = config->getLoadPreviousSession() && StateManager::getInstance()->loadState(isUsingGUI());
	if (!didLoadFromSave)
		core->setDefaultMixerSetup();

	core->initializeDevicePlayback();

	StateManager::getInstance()->startAutoStore(config->getSessionStoreIntervalSecs() * 1000);

	if (useGUI)
		MainView::getInstance()->setAppLoaded(true);
}

void Main::shutdown()
{
	stopWLAN();

	if (initialised)
	{
		AudioCore::getInstance()->stopDevicePlayback();
		StateManager::getInstance()->stopAutoStore();

		mainWindow = nullptr;

		StateManager::getInstance()->storeState();

		API::APICore::getInstance()->shutdown();
		AudioCore::getInstance()->shutdown();

		{
			ScopedPointer<LookAndFeel> lf(&LookAndFeel::getDefaultLookAndFeel());
			LookAndFeel::setDefaultLookAndFeel(nullptr);
		}

		Singleton::deleteInstances();
		FontAwesome::deleteInstance();
	}

	Bootstrapper::deleteInstance();
}


void Main::systemRequestedQuit()
{
	MessageManager::callAsync([=]() {
		quit();
	});
}

ValueTree Main::getState() const
{
	ValueTree state("WOAP");
	state.addChild(AudioCore::getInstance()->getState(), -1, nullptr);
	state.addChild(AccessControlCore::getInstance()->getState(), -1, nullptr);

	return state;
}

void Main::setState(const ValueTree& state)
{
	if (!state.hasType("WOAP"))
		return;

	AudioCore::getInstance()->setState(state.getChildWithName("AUDIOCORE"));
	AccessControlCore::getInstance()->setState(state.getChildWithName("ACCESSCONTROLCORE"));
}

void Main::startWLANIfAllowed()
{
	ConfigurationManager* config = ConfigurationManager::getInstance();
	
	if (config->isWLANEnabled())
	{
#ifdef JUCE_WINDOWS
		executeCommand(L"netsh wlan set hostednetwork mode=allow ssid=\""
			+ config->getWLANName() + "\" key=\""
			+ config->getWLANPassword()
			+ "\" keyUsage=persistent && netsh wlan start hostednetwork");
#else
		jassertfalse;
#endif

		wlanStarted = true;
	}
}

void Main::stopWLAN()
{
	if (wlanStarted)
	{
#ifdef JUCE_WINDOWS
		executeCommand(L"netsh wlan stop hostednetwork");
#else
		jassertfalse;
#endif

		wlanStarted = false;
	}
}

START_JUCE_APPLICATION (Main)