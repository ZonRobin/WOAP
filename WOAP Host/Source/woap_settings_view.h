#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_plugin_manager.h"

namespace WOAP
{
	namespace GUI
	{
		class SoloBusSettingsComponent;
		class ConfigurationEditor;
		class LoadingSpinner;
		
		class SettingsView : public Component, public Button::Listener, public PluginManager::Listener
		{
		public:
			class PluginImporter;
			class PAGTemplatesImporter;

			SettingsView();
			~SettingsView();

			void resized() override;
			void paint(Graphics& g) override;

			virtual void buttonClicked(Button*) override;

			void startPluginImport();

			virtual void pluginTypesScanStarted() override;
			virtual void pluginTypesScanned(int newNumPlugins) override;
			virtual void pluginTypeAdded(const PluginDescription&) {};

		protected:
			ScopedPointer<TextButton> scanForPluginsButton;
			ScopedPointer<TextButton> importPluginsButton;
			ScopedPointer<PluginImporter> pluginImporter;
			ScopedPointer<LoadingSpinner> pluginLoadingSpinner;

			ScopedPointer<TextButton> importPagTemplatesButton;
			ScopedPointer<PAGTemplatesImporter> pagTemplatesImporter;

			ScopedPointer<SoloBusSettingsComponent> soloBusSettings;
			ScopedPointer<ConfigurationEditor> configEditor;
			ScopedPointer<AudioDeviceSelectorComponent> audioDeviceSelector;
			ScopedPointer<LookAndFeel> audioDeviceSelectorLf;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsView)

		public:
			class PluginImporter : public Thread
			{
			public:
				enum class State
				{
					ThreadStarted,
					PluginsSelected,
					CancelledByUser,
					NothingToImport,
					ImportFailed,
					PluginEditorCreated,
					UserWantsImport,
					UserRejectedImport,
					PluginImported
				};

				PluginImporter();
				~PluginImporter();

				virtual void run() override;

				void stateChanged(State oldState);
				void setState(State newState);

				void startImport();
				void importNextPlugin();
				void askUserToImport();
				void importPlugin();

			protected:
				ScopedPointer<PluginManager::PluginImportData> importData;
				ScopedPointer<DocumentWindow> pluginEditor;

				Array<File> pluginsToImport;
				State state;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginImporter)
			};

			class PAGTemplatesImporter
			{
			public:
				PAGTemplatesImporter();
				~PAGTemplatesImporter();

				void startImport();
				bool importSingleTemplate(const File& tmpl);
				int getPluginUid(const XmlElement* data) const;
				String getPluginName(const XmlElement* data) const;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PAGTemplatesImporter)
			};
		};
	}
}