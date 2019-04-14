#pragma once

#include "woap_api_listener_brides_osc_bridge_base.h"
#include "woap_audio_processor_player_double.h"
#include "woap_parameter.h"

namespace WOAP
{
	namespace API
	{
		namespace ListenerBridges
		{
			class PluginListenerBridge : public OSCBridgeBase<AudioProcessorPlayerDouble>, public AudioProcessorPlayerDouble::Listener
			{
			public:
				PluginListenerBridge(AudioProcessorPlayerDouble& plugin, OSC::RemoteClient& client);
				virtual ~PluginListenerBridge();

				virtual void nameChanged(AudioProcessorPlayerDouble*) override;

				virtual void aboutToDelete(AudioProcessorPlayerDouble* plugin) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginListenerBridge)
			};

			class PluginBypassListenerBridge : public OSCBridgeBase<AudioProcessorPlayerDouble>, public WOAP::Parameter::Listener
			{
			public:
				PluginBypassListenerBridge(AudioProcessorPlayerDouble& plugin, OSC::RemoteClient& client);
				virtual ~PluginBypassListenerBridge();

				virtual void parameterChanged(Parameter*) override;

				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginBypassListenerBridge)
			};

			class PluginActiveListenerBridge : public OSCBridgeBase<AudioProcessorPlayerDouble>, public WOAP::Parameter::Listener
			{
			public:
				PluginActiveListenerBridge(AudioProcessorPlayerDouble& plugin, OSC::RemoteClient& client);
				virtual ~PluginActiveListenerBridge();

				virtual void parameterChanged(Parameter*) override;

				virtual void parameterWillBeDeleted(Parameter*) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginActiveListenerBridge)
			};

			class PluginParameterListenerBridge : public OSCBridgeBase<AudioProcessorPlayerDouble>, public AudioProcessorPlayerDouble::ParameterListener
			{
			public:
				PluginParameterListenerBridge(AudioProcessorPlayerDouble& plugin, OSC::RemoteClient& client);
				virtual ~PluginParameterListenerBridge();
				
				virtual void audioProcessorParameterChanged(AudioProcessorPlayerDouble*, int parameterIndex, float newValue) override;

				virtual void audioProcessorParameterChangeGestureBegin(AudioProcessorPlayerDouble*, int parameterIndex) override;

				virtual void audioProcessorParameterChangeGestureEnd(AudioProcessorPlayerDouble*, int parameterIndex) override;

				virtual void aboutToDelete(AudioProcessorPlayerDouble* plugin) override;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameterListenerBridge)
			};
		}
	}
}
