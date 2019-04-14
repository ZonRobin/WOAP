#include "woap_main.h"
#include "woap_main_component.h"
#include "woap_look_and_feel.h"

const String Main::PAGVersion = "1.0";

Main::Main()
{
	
}

const String Main::getApplicationName() 
{ 
	return ProjectInfo::projectName; 
}

const String Main::getApplicationVersion() 
{ 
	return ProjectInfo::versionString; 
}

bool Main::moreThanOneInstanceAllowed() 
{ 
	return true; 
}

void Main::initialise(const String&)
{
	mainWindow = new MainWindow(getApplicationName());
}

void Main::shutdown()
{
	mainWindow = nullptr;
	commandManager = nullptr;
}

void Main::systemRequestedQuit()
{
	if (AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Confirm exit", "Are you sure to exit?", "Yes", "No") == 1)
		quit();
}

void Main::anotherInstanceStarted(const String&)
{

}

ScopedPointer<ApplicationCommandManager> Main::commandManager(nullptr);
ApplicationCommandManager& Main::getApplicationCommandManager()
{
	if (Main::commandManager == nullptr) {
		Main::commandManager = new ApplicationCommandManager();
	}

	return *Main::commandManager;
}

void Main::invokeCommand(CommandID commandId, bool asynchronously)
{
	Main::getApplicationCommandManager().invoke(ApplicationCommandTarget::InvocationInfo(commandId), asynchronously);
}

Main::MainWindow::MainWindow(String name) : DocumentWindow(name,
	Desktop::getInstance().getDefaultLookAndFeel()
	.findColour(ResizableWindow::backgroundColourId),
	DocumentWindow::allButtons)
{
	LookAndFeel::setDefaultLookAndFeel(lookAndFeel = new WOAPLookAndFeel());

	setUsingNativeTitleBar(true);
	setContentOwned(new MainComponent(), true);
	setResizable(false, false);
	centreWithSize(getWidth(), getHeight());
	setVisible(true);
}

Main::MainWindow::~MainWindow()
{
	clearContentComponent();
	LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void Main::MainWindow::closeButtonPressed()
{
	JUCEApplication::getInstance()->systemRequestedQuit();
}

START_JUCE_APPLICATION(Main)