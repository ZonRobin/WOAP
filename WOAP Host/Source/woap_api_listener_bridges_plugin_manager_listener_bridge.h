#pragma once

#include "woap_api_listener_brides_osc_bridge_base.h"
#include "woap_plugin_manager.h"

namespace WOAP
{
	namespace API
	{
		namespace ListenerBridges
		{
			class PluginManagerListenerBridge : public OSCBridgeBase<PluginManager>, public PluginManager::Listener
			{
			public:
				PluginManagerListenerBridge(PluginManager& pluginManager, OSC::RemoteClient& client);
				virtual ~PluginManagerListenerBridge();

				virtual void pluginTypesScanStarted() override;
				virtual void pluginTypesScanned(int newNumPlugins) override;
				virtual void pluginTypeAdded(const PluginDescription&) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerListenerBridge)
			};
		}
	}
}
