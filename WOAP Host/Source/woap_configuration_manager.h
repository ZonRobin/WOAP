#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"
#include "woap_stateful.h"

namespace WOAP
{
	class ConfigurationManager : public Stateful
	{
	public:
		struct Configuration : public Stateful
		{
			Value loadPreviousSession;
			Value sessionStoreIntervalSecs;

			Value anonymousLoginEnabled;
			Value adminPassword;

			Value WLANEnabled;
			Value WLANName;
			Value WLANPassword;

			Configuration() {}

			Configuration(const Configuration& other)
			{
				loadPreviousSession = other.loadPreviousSession.getValue();
				sessionStoreIntervalSecs = other.sessionStoreIntervalSecs.getValue();

				anonymousLoginEnabled = other.anonymousLoginEnabled.getValue();
				adminPassword = other.adminPassword.getValue();

				WLANEnabled = other.WLANEnabled.getValue();
				WLANName = other.WLANName.getValue();
				WLANPassword = other.WLANPassword.getValue();
			}

			ValueTree getState() const
			{
				ValueTree state("CONFIGURATION");
				state.setProperty("loadPreviousSession", loadPreviousSession, nullptr);
				state.setProperty("sessionStoreIntervalSecs", sessionStoreIntervalSecs, nullptr);
				state.setProperty("anonymousLoginEnabled", anonymousLoginEnabled, nullptr);
				state.setProperty("adminPassword", adminPassword, nullptr);
				state.setProperty("WLANEnabled", WLANEnabled, nullptr);
				state.setProperty("WLANName", WLANName, nullptr);
				state.setProperty("WLANPassword", WLANPassword, nullptr);

				return state;
			}

			void setState(const ValueTree& state)
			{
				if (!state.hasType("CONFIGURATION"))
					return;

				loadPreviousSession = StateHelper::getBoolProperty(state, "loadPreviousSession", loadPreviousSession.getValue());
				sessionStoreIntervalSecs = StateHelper::getIntProperty(state, "sessionStoreIntervalSecs", sessionStoreIntervalSecs.getValue());
				anonymousLoginEnabled = StateHelper::getBoolProperty(state, "anonymousLoginEnabled", anonymousLoginEnabled.getValue());
				adminPassword = StateHelper::getStringProperty(state, "adminPassword", adminPassword.getValue());
				WLANEnabled = StateHelper::getBoolProperty(state, "WLANEnabled", WLANEnabled.getValue());
				WLANName = StateHelper::getStringProperty(state, "WLANName", WLANName.getValue());
				WLANPassword = StateHelper::getStringProperty(state, "WLANPassword", WLANPassword.getValue());
			}
		};

		class Listener
		{
		public:
			virtual void configurationChanged(const ConfigurationManager::Configuration& oldConfig, const ConfigurationManager::Configuration& newConfig) = 0;
		};

		const Configuration& getConfiguration() const { return *config; }

		void setConfiguration(const Configuration& c);
		bool setConfiguration(const Configuration& c, String& errorMessage);

		bool getLoadPreviousSession() const { return config->loadPreviousSession.getValue(); }
		int getSessionStoreIntervalSecs() const { return config->sessionStoreIntervalSecs.getValue(); }

		bool isAnonymousLoginEnabled() const { return config->anonymousLoginEnabled.getValue(); }
		String getAdminPassword() const { return config->adminPassword.getValue(); }

		bool isWLANEnabled() const { return config->WLANEnabled.getValue(); }
		String getWLANName() const { return config->WLANName.getValue(); }
		String getWLANPassword() const { return config->WLANPassword.getValue(); }

		bool saveConfig() const;
		bool saveConfigToFile(const File& storeFile) const;

		bool loadConfig();
		void loadConfigFromFile(const File& file);

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;

		void addListener(Listener* l) { listeners.add(l); }
		void removeListener(Listener* l) { listeners.remove(l); }

	protected:
		Configuration getDefaultConfiguration() const;

		ScopedPointer<Configuration> config;
		ListenerList<Listener> listeners;

	private:
		SINGLETON(ConfigurationManager)
	};
}


