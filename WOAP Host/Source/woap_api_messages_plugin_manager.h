#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_messages_validatable_osc_message.h"
#include "woap_api_messages_validatable_osc_message_with_id.h"

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class PluginManagerGetPluginTypes : public ValidatableOSCMessageWithId
			{
			public:
				PluginManagerGetPluginTypes(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				PluginManagerGetPluginTypes()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/getPluginTypes");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerGetPluginTypes)
			};

			class PluginManagerPluginTypes : public ValidatableOSCMessageWithId
			{
			public:
				PluginManagerPluginTypes(const OSCMessage& msg)
					: ValidatableOSCMessageWithId(msg)
				{

				}

				PluginManagerPluginTypes(const PluginManagerGetPluginTypes& msg, const Array<PluginDescription*>& pluginTypes)
				{
					var types;
					types.resize(pluginTypes.size());

					int index = 0;
					for (PluginDescription* description : pluginTypes)
					{
						var type;
						type.append(description->uid);
						type.append(description->name);
						type.append(description->manufacturerName);
						types[index++] = type;
					}

					initializeResponseWithArgs(msg, OSCArgument(JSON::toString(types)));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::string,	/* pluginTypesJSON */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/pluginTypes");
				}

				inline var getPluginTypes()
				{
					return JSON::fromString(getArgumentString(0));
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerPluginTypes)
			};

			class PluginManagerScanPluginTypes : public ValidatableOSCMessage
			{
			public:
				PluginManagerScanPluginTypes(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerScanPluginTypes()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/scanPluginTypes");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerScanPluginTypes)
			};

			class PluginManagerScanPluginTypesIfNeeded : public ValidatableOSCMessage
			{
			public:
				PluginManagerScanPluginTypesIfNeeded(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerScanPluginTypesIfNeeded()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/scanPluginTypesIfNeeded");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerScanPluginTypesIfNeeded)
			};

			class PluginManagerPluginTypesScanStarted : public ValidatableOSCMessage
			{
			public:
				PluginManagerPluginTypesScanStarted(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerPluginTypesScanStarted()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/pluginManagerPluginTypesScanStarted");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerPluginTypesScanStarted)
			};

			class PluginManagerPluginTypesScanned : public ValidatableOSCMessage
			{
			public:
				PluginManagerPluginTypesScanned(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerPluginTypesScanned(int newNumPlugins)
				{
					initializeWithArgs(OSCArgument(newNumPlugins));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* newNumPlugins */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/pluginManagerPluginTypesScanned");
				}

				inline int getNewNumPlugins()
				{
					return getArgumentInt(0);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerPluginTypesScanned)
			};

			class PluginManagerPluginTypeAdded : public ValidatableOSCMessage
			{
			public:
				PluginManagerPluginTypeAdded(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerPluginTypeAdded(const PluginDescription& desc)
				{
					initializeWithArgs(OSCArgument(desc.uid), OSCArgument(desc.name), OSCArgument(desc.manufacturerName));
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>({
						OSCTypes::int32,	/* pluginId */
						OSCTypes::string,	/* pluginName */
						OSCTypes::string,	/* pluginManufacturerName */
					});
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/pluginManagerPluginTypeAdded");
				}

				inline int getPluginId()
				{
					return getArgumentInt(0);
				}

				inline String getPluginName()
				{
					return getArgumentString(1);
				}

				inline String getPluginManufacturerName()
				{
					return getArgumentString(2);
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerPluginTypeAdded)
			};

			class PluginManagerAddListener : public ValidatableOSCMessage
			{
			public:
				PluginManagerAddListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerAddListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/addListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerAddListener)
			};

			class PluginManagerRemoveListener : public ValidatableOSCMessage
			{
			public:
				PluginManagerRemoveListener(const OSCMessage& msg)
					: ValidatableOSCMessage(msg)
				{

				}

				PluginManagerRemoveListener()
				{
					initializeMessage();
				}

				inline virtual String getValidAddress() const
				{
					return getAddress();
				}

				inline static String getAddress()
				{
					return String("/pluginManager/removeListener");
				}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginManagerRemoveListener)
			};
		}
	}
}