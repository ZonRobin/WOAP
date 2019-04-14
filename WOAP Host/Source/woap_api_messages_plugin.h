#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_api_messages_validatable_osc_message_with_id.h"
#include "woap_api_messages_parameter_message.h"
#include "woap_functions.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class PluginGetDetail : public ValidatableOSCMessageWithId
			{
			public:
				PluginGetDetail(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginGetDetail(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/getDetail");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginGetDetail)
			};

			class PluginDetail : public ValidatableOSCMessageWithId
			{
			public:
				PluginDetail(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginDetail(const PluginGetDetail& msg, const Identifiable* plugin, const String& name, const String& type, int typeId, bool bypassed, bool active)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeResponseWithArgs(
						msg,
						OSCArgument(name),
						OSCArgument(type),
						OSCArgument(typeId),
						OSCArgument(boolToInt(bypassed)),
						OSCArgument(boolToInt(active))
					);
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* Name */
						OSCTypes::string,	/* Type */
						OSCTypes::int32,	/* typeId */
						OSCTypes::int32,	/* Bypassed */
						OSCTypes::int32,	/* Active */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/detail");
				}

				inline String getName() const
				{
					return getArgumentString(0);
				}

				inline String getType() const
				{
					return getArgumentString(1);
				}

				inline int getTypeId() const
				{
					return getArgumentInt(2);
				}

				inline bool getBypassedState() const
				{
					return getArgumentBool(3);
				}

				inline bool getActiveState() const
				{
					return getArgumentBool(4);
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginDetail)
			};

			class PluginGetParameterValues : public ValidatableOSCMessageWithId
			{
			public:
				PluginGetParameterValues(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginGetParameterValues(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/getParameterValues");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginGetParameterValues)
			};

			class PluginGetParameters : public ValidatableOSCMessageWithId
			{
			public:
				PluginGetParameters(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginGetParameters(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/getParameters");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginGetParameters)
			};

			class PluginAddListener : public ValidatableOSCMessage
			{
			public:
				PluginAddListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginAddListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/addListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAddListener)
			};

			class PluginRemoveListener : public ValidatableOSCMessage
			{
			public:
				PluginRemoveListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginRemoveListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/removeListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginRemoveListener)
			};

			class PluginAddParameterListener : public ValidatableOSCMessage
			{
			public:
				PluginAddParameterListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginAddParameterListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/addParameterListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAddParameterListener)
			};

			class PluginRemoveParameterListener : public ValidatableOSCMessage
			{
			public:
				PluginRemoveParameterListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginRemoveParameterListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/removeParameterListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginRemoveParameterListener)
			};

			class PluginAddBypassListener : public ValidatableOSCMessage
			{
			public:
				PluginAddBypassListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginAddBypassListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/addBypassListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAddBypassListener)
			};

			class PluginRemoveBypassListener : public ValidatableOSCMessage
			{
			public:
				PluginRemoveBypassListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginRemoveBypassListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/removeBypassListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginRemoveBypassListener)
			};

			class PluginAddActiveListener : public ValidatableOSCMessage
			{
			public:
				PluginAddActiveListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginAddActiveListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/addActiveListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginAddActiveListener)
			};

			class PluginRemoveActiveListener : public ValidatableOSCMessage
			{
			public:
				PluginRemoveActiveListener(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginRemoveActiveListener(const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/removeActiveListener");
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginRemoveActiveListener)
			};

			class PluginSetParameterValue : public ValidatableOSCMessage
			{
			public:
				PluginSetParameterValue(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginSetParameterValue(const Identifiable* plugin, int paramIndex, float newValue)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(paramIndex), OSCArgument(newValue));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* Parameter index */
						OSCTypes::float32,	/* NewValue */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/setParameterValue");
				}

				inline int getParameterIndex() const
				{
					return getArgumentInt(0);
				}

				inline float getNewValue() const
				{
					return getArgumentFloat(1);
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSetParameterValue)
			};

			class PluginParameterValueChanged : public ValidatableOSCMessage
			{
			public:
				PluginParameterValueChanged(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginParameterValueChanged(const Identifiable* plugin, int paramIndex, float newValue, const String& valueText)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(paramIndex), OSCArgument(newValue), OSCArgument(valueText));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* Parameter index */
						OSCTypes::float32,	/* NewValue */
						OSCTypes::string	/* ValueText */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/parameterValueChanged");
				}

				inline int getParameterIndex() const
				{
					return getArgumentInt(0);
				}

				inline float getNewValue() const
				{
					return getArgumentFloat(1);
				}

				inline String getNewValueText() const
				{
					return getArgumentString(2);
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameterValueChanged)
			};

			class PluginParameters : public ValidatableOSCMessageWithId
			{
			public:
				PluginParameters(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginParameters(const PluginGetParameters& req, const Identifiable* plugin, Array<AudioProcessorPlayerDouble::ParameterInfo> parametersInfo)
					: pluginId(plugin->getId().toDashedString())
				{
					var params;
					params.resize(parametersInfo.size());

					int index = 0;
					for (AudioProcessorPlayerDouble::ParameterInfo paramInfo : parametersInfo)
					{
						params[index++] = var({(var)paramInfo.index, (var)paramInfo.ref, (var)paramInfo.value, (var)paramInfo.valueText});
					}

					initializeResponseWithArgs(req, OSCArgument(JSON::toString(params)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* Values */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/parameters");
				}

				inline Array<AudioProcessorPlayerDouble::ParameterInfo> getParameters() const
				{
					jassertfalse; /* Not supported */
					return Array<AudioProcessorPlayerDouble::ParameterInfo>();
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameters)
			};

			class PluginParameterValues : public ValidatableOSCMessageWithId
			{
			public:
				PluginParameterValues(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessageWithId(msg)
				{

				}

				PluginParameterValues(const PluginGetParameterValues& req, const Identifiable* plugin, var parameterValues)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeResponseWithArgs(req, OSCArgument(JSON::toString(parameterValues)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* Values */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/parameterValues");
				}

				inline var getParameterValues() const
				{
					jassertfalse; /* Not supported */
					return var();
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameterValues)
			};

			class PluginSetName : public ValidatableOSCMessage
			{
			public:
				PluginSetName(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginSetName(const Identifiable* plugin, const String& newName)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* NewName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/setName");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSetName)
			};

			class PluginNameChanged : public ValidatableOSCMessage
			{
			public:
				PluginNameChanged(const OSCMessage& msg, const Identifiable* plugin)
					: pluginId(plugin->getId().toDashedString()), ValidatableOSCMessage(msg)
				{

				}

				PluginNameChanged(const Identifiable* plugin, const String& newName)
					: pluginId(plugin->getId().toDashedString())
				{
					initializeWithArgs(OSCArgument(newName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string	/* NewName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(pluginId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/nameChanged");
				}

				inline String getNewName() const
				{
					return getArgumentString(0);
				}

			protected:
				String pluginId;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginNameChanged)
			};

			class PluginSetBypass : public ParameterBoolMessage
			{
			public:
				PluginSetBypass(const OSCMessage& msg, const Identifiable* plugin)
					: ParameterBoolMessage(msg, plugin)
				{

				}

				PluginSetBypass(const Identifiable* plugin, bool newState)
					: ParameterBoolMessage(plugin)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/setBypass");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSetBypass)
			};

			class PluginBypassChanged : public ParameterBoolMessage
			{
			public:
				PluginBypassChanged(const OSCMessage& msg, const Identifiable* plugin)
					: ParameterBoolMessage(msg, plugin)
				{

				}

				PluginBypassChanged(const Identifiable* owner, bool newState)
					: ParameterBoolMessage(owner)
				{
					initializeWithArgs(OSCArgument(boolToInt(newState)));
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/bypassChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginBypassChanged)
			};

			class PluginSetActive : public ParameterBoolMessage
			{
			public:
				PluginSetActive(const OSCMessage& msg, const Identifiable* plugin)
					: ParameterBoolMessage(msg, plugin)
				{

				}

				PluginSetActive(const Identifiable* plugin, bool newState)
					: ParameterBoolMessage(plugin)
				{
					initializeMessage(newState);
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/setActive");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSetActive)
			};

			class PluginActiveChanged : public ParameterBoolMessage
			{
			public:
				PluginActiveChanged(const OSCMessage& msg, const Identifiable* plugin)
					: ParameterBoolMessage(msg, plugin)
				{

				}

				PluginActiveChanged(const Identifiable* plugin, bool newState)
					: ParameterBoolMessage(plugin)
				{
					initializeWithArgs(OSCArgument(boolToInt(newState)));
				}

				inline virtual String getValidAddress() const
				{
					return getAddress(ownerId);
				}

				inline static String getAddress(const String& pluginId)
				{
					return String("/plugin/" + pluginId + "/activeChanged");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginActiveChanged)
			};
		}
	}
}
