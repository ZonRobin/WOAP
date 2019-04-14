#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_configuration_manager.h"

namespace WOAP
{
	namespace GUI
	{
		class ConfigurationEditor : public Component, public Button::Listener, public ConfigurationManager::Listener
		{
		public:
			ConfigurationEditor();
			~ConfigurationEditor();
			
			void fillPanel();
			void resized() override;

			void saveConfiguration();

			virtual void buttonClicked(Button*) override;
			virtual void configurationChanged(const ConfigurationManager::Configuration& oldConfig, const ConfigurationManager::Configuration& newConfig) override;

		protected:
			ScopedPointer<ConfigurationManager::Configuration> config;

			ScopedPointer<PropertyPanel> propertyPanel;
			ScopedPointer<TextButton> saveButton;
			ScopedPointer<TextButton> resetButton;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigurationEditor)
		};
	}
}