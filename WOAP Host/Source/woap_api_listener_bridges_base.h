#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace API
	{
		namespace ListenerBridges
		{
			class IListenerBridge
			{
			public:
				IListenerBridge() {}
				virtual ~IListenerBridge() {}

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IListenerBridge)
			};

			template <class TrackedInstanceType>
			class Base : public IListenerBridge
			{
			public:
				Base(TrackedInstanceType& trackedInst)
					: trackedInstance(trackedInst)
				{

				}

				virtual ~Base() override
				{

				}

				TrackedInstanceType& getTrackedInstance() const
				{
					return trackedInstance;
				}

			protected:
				TrackedInstanceType& trackedInstance;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Base)
			};
		}
	}
}
