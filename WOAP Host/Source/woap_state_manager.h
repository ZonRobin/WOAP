#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"
#include "woap_api_consumer.h"
#include "woap_bootstrapper.h"

namespace WOAP
{
	class StateManager : public API::APIConsumer
	{
	public:
		enum MessageType
		{
			SaveState,
			LoadState,
			GetState,
			SetState
		};

		enum Permission
		{
			_BaseId = 1300,
			HandleStateInternal,
			HandleStateExternal
		};

		void startAutoStore(int storeInterval);
		void stopAutoStore();

		bool loadState(bool showAlertOnError) const;
		bool loadStateFromFile(const File& file, bool showAlertOnError) const;
		void stateLoadingFailed() const;

		bool storeState() const;
		bool storeState(const File& storeFile) const;

		XmlElement* getState() const;
		void getStateString(OutputStream& stream) const;

		bool setState(const XmlElement* state, bool showAlertOnError) const;
		bool setState(const String& state, bool showAlertOnError) const;

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const override;
		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

	protected:
		class StoreWorker : public Thread
		{
		public:
			StoreWorker(const File& storeFile);
			~StoreWorker();

			virtual void run() override;

			void setStoreIntervalMs(int newIntervalMs) { storeIntervalMs = jmax(5000, newIntervalMs); }
			void setStorePath(const File& newFile) { saveFile = newFile; }

		protected:
			int storeIntervalMs = 5000;
			File saveFile;
			CriticalSection fileWriterLock;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StoreWorker)
		};

		ScopedPointer<StoreWorker> storeWorker;

	private:
		const static Bootstrapper::Initializer bootstrapInitializer;
		SINGLETON(StateManager)
	};
}