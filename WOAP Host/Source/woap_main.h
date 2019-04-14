#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_stateful.h"

namespace WOAP
{
	class AppCore;
	
	namespace GUI
	{
		class MainWindow;
		class MainView;
	}

	class Main : public JUCEApplication, public Stateful
	{
	public:
		Main();
		virtual ~Main();

		const String getApplicationName() override { return ProjectInfo::projectName; }
		const String getApplicationVersion() override { return ProjectInfo::versionString; }
		const String& getDeviceVersion() const { return deviceVersion; }
		const Uuid& getDeviceId() const { return deviceId; }
		const Uuid& getProcessId() const { return processId; }
		bool isKioskMode() const { return kioskMode; }
		bool isUsingGUI() const { return useGUI; }

		File getApplicationDirectory() const;
		File getHostApplicationDirectory() const;
		File getPluginsDirectory() const;
		File getPAGTemplatesDirectory() const;
		File getApplicationStateFile() const;
		File getApplicationConfigFile() const;
		File getAudioDeviceSettingsFile() const;
		File getPluginBlacklistFile() const;
		File getPluginListFile() const;

		static Main* getInstanceCast() noexcept { return static_cast<Main*>(getInstance()); }

		bool moreThanOneInstanceAllowed() override;

		void initialise(const String& commandLine) override;
		void initialiseGui();
		void loadApp();

		void shutdown() override;
		void systemRequestedQuit() override;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;

		ApplicationCommandManager& getCommandManager() { return *commandManager; }

		void startWLANIfAllowed();
		void stopWLAN();

	private:
		bool initialised = false;
		bool wlanStarted = false;

		const String deviceVersion;
		const Uuid deviceId;
		const Uuid processId;
		const bool kioskMode;
		const bool kioskModeToggle;
		const bool useGUI;

		ScopedPointer<GUI::MainWindow> mainWindow;
		ScopedPointer<ApplicationCommandManager> commandManager;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Main)
	};
}
