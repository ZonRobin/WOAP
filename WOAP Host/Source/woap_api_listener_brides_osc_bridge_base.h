#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_api_listener_bridges_base.h"
#include "woap.h"

namespace WOAP
{
	namespace API
	{
		namespace OSC
		{
			class RemoteClient;
		}

		namespace ListenerBridges
		{
			template <class TrackedInstanceType>
			class OSCBridgeBase : public Base<TrackedInstanceType>
			{
			public:
				OSCBridgeBase(TrackedInstanceType& trackedInst, OSC::RemoteClient& cl)
					: Base<TrackedInstanceType>(trackedInst), client(cl)
				{
					client.addListenerBridge(this);
				}

				virtual ~OSCBridgeBase()
				{
					
				}

				OSC::RemoteClient& getClient() const
				{
					return client;
				}

				bool equalsTo(const TrackedInstanceType& trackedInst, const OSC::RemoteClient& cl) const
				{
					return &trackedInstance == &trackedInst && &client == &cl;
				}

				bool equalsTo(const OSCBridgeBase<TrackedInstanceType>& otherListener) const
				{
					return &trackedInstance == &(otherListener.trackedInstance) && &client == &(otherListener.client);
				}

				template<class ListenerType>
				static bool addToList(ListenerList<ListenerType>& listeners, ListenerType* listenerToAdd)
				{
					OSCBridgeBase<TrackedInstanceType>* listenerOSCBase = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listenerToAdd);
					jassert(listenerOSCBase != nullptr);

					for (ListenerType* listener : listeners.getListeners())
					{
						if (OSCBridgeBase<TrackedInstanceType>* bridge = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listener))
						{
							if (bridge->equalsTo(*listenerOSCBase))
							{
								bridge->client.removeListenerBridge(bridge);
								return false;
							}
						}
					}

					listeners.add(listenerToAdd);
					return true;
				}

				template<class ListenerType>
				static bool addToList(LockedListenerList<ListenerType>& listeners, ListenerType* listenerToAdd)
				{
					OSCBridgeBase<TrackedInstanceType>* listenerOSCBase = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listenerToAdd);
					jassert(listenerOSCBase != nullptr);

					const ScopedLock lock(listeners.getListeners().getLock());

					for (ListenerType* listener : listeners.getListeners())
					{
						if (OSCBridgeBase<TrackedInstanceType>* bridge = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listener))
						{
							if (bridge->equalsTo(*listenerOSCBase))
							{
								bridge->client.removeListenerBridge(bridge);
								return false;
							}
						}
					}

					listeners.add(listenerToAdd);
					return true;
				}

				template<class ListenerType>
				static bool removeFromList(ListenerList<ListenerType>& listeners, const TrackedInstanceType& trackedInst, const OSC::RemoteClient& cl)
				{
					for (ListenerType* listener : listeners.getListeners())
					{
						if (OSCBridgeBase<TrackedInstanceType>* bridge = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listener))
						{
							if (bridge->equalsTo(trackedInst, cl))
							{
								listeners.remove(dynamic_cast<ListenerType*>(bridge));
								bridge->client.removeListenerBridge(bridge);
								return true;
							}
						}
					}
			
					return false;
				}

				template<class ListenerType>
				static bool removeFromList(LockedListenerList<ListenerType>& listeners, const TrackedInstanceType& trackedInst, const OSC::RemoteClient& cl)
				{
					const ScopedLock lock(listeners.getListeners().getLock());

					for (ListenerType* listener : listeners.getListeners())
					{
						if (OSCBridgeBase<TrackedInstanceType>* bridge = dynamic_cast<OSCBridgeBase<TrackedInstanceType>*>(listener))
						{
							if (bridge->equalsTo(trackedInst, cl))
							{
								listeners.remove(dynamic_cast<ListenerType*>(bridge));
								bridge->client.removeListenerBridge(bridge);
								return true;
							}
						}
					}

					return false;
				}

			protected:
				OSC::RemoteClient& client;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCBridgeBase)
			};
		}
	}
}
