#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"
#include "woap_api_consumer.h"
#include "woap_bootstrapper.h"
#include "woap.h"

namespace WOAP
{
	class PluginManager : public API::APIConsumer
	{
	public:
		enum MessageType
		{
			GetPluginTypes,
			ScanPluginTypes,
			ScanPluginTypesIfNeeded,
			AddListener,
			RemoveListener
		};

		enum Permission
		{
			_BaseId = 1200,
			View,
			Scan
		};

		struct PluginImportData
		{
			enum ReturnCode
			{
				Ok = 0,
				NoPluginFound,
				MoreThanSinglePlugin,
				UnknownError
			};

			ReturnCode returnCode;
			ScopedPointer<PluginDescription> pluginDescription;
			ScopedPointer<AudioPluginInstance> pluginInstance;

			PluginImportData(ReturnCode code, PluginDescription* desc, AudioPluginInstance* instance)
				: returnCode(code), pluginDescription(desc), pluginInstance(instance)
			{

			}

			static PluginImportData* Fail(ReturnCode code)
			{
				return new PluginImportData(code, nullptr, nullptr);
			}

			static PluginImportData* Success(PluginDescription* desc, AudioPluginInstance* instance)
			{
				return new PluginImportData(ReturnCode::Ok, desc, instance);
			}
		};

		class Listener
		{
		public:
			virtual void pluginTypesScanStarted() {};
			virtual void pluginTypesScanned(int newNumPlugins) = 0;
			virtual void pluginTypeAdded(const PluginDescription&) = 0;
		};

		class ScanWorker : public Thread
		{
		public:
			ScanWorker(PluginManager* pm) : pluginManager(pm), Thread("Plugin scan worker") {}
			~ScanWorker() {}

			virtual void run() override;

		protected:
			PluginManager* pluginManager;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScanWorker)
		};

		AudioPluginFormatManager* getAudioPluginFormatManager() { return &audioPluginFormatManager; }

		void scanPluginTypes();
		void scanPluginTypesIfNeeded();

		const KnownPluginList& getKnownPluginList() const { return knownPlugins; }
		bool isScanned() const { return scanRequested; }
		const PluginDescription* getPluginType(int pluginId) const;

		void saveKnownPluginList() const;
		void loadKnownPluginList();

		PluginImportData* loadPluginByPath(const String& path);
		bool importPlugin(const PluginDescription& description);

		void addListener(Listener* listener) { listeners.add(listener); }
		void removeListener(Listener* listener) { listeners.remove(listener); }

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const;
		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

	protected:
		CriticalSection scanLock;
		KnownPluginList knownPlugins;
		bool scanRequested = false;

		ListenerList<Listener> listeners;
		AudioPluginFormatManager audioPluginFormatManager;
		ScopedPointer<ScanWorker> scanWorker;

	private:
		const static Bootstrapper::Initializer bootstrapInitializer;
		SINGLETON(PluginManager)
	};
}
