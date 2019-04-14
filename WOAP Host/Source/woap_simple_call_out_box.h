#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace GUI
	{
		class SimpleCallOutBox : public ModalComponentManager::Callback, private Timer
		{
		public:
			SimpleCallOutBox(Component* c, const Rectangle<int>& area, Component* parent = nullptr)
				: content(c), callout(*c, area, parent)
			{
				callout.setVisible(true);
				callout.enterModalState(true, this);
				startTimer(200);
			}

			static SimpleCallOutBox* open(Component* c, const Rectangle<int>& area, Component* parent = nullptr)
			{
				return new SimpleCallOutBox(c, area, parent);
			}

			static void close(WeakReference<SimpleCallOutBox>& callOut)
			{
				if (callOut != nullptr)
					callOut->callout.dismiss();
			}

			void modalStateFinished(int) override {}

			void timerCallback() override
			{
				if (!Process::isForegroundProcess())
					callout.dismiss();
			}

			void setDismissalMouseClicksAreAlwaysConsumed(bool shouldAlwaysBeConsumed) noexcept
			{
				callout.setDismissalMouseClicksAreAlwaysConsumed(shouldAlwaysBeConsumed);
			}

			void setArrowSize(float newSize)
			{
				callout.setArrowSize(newSize);
			}

			void updatePosition(const Rectangle<int>& newAreaToPointTo, const Rectangle<int>& newAreaToFitIn)
			{
				callout.updatePosition(newAreaToPointTo, newAreaToFitIn);
			}

		protected:
			ScopedPointer<Component> content;
			CallOutBox callout;

		private:
			JUCE_DECLARE_WEAK_REFERENCEABLE(SimpleCallOutBox)
			JUCE_DECLARE_NON_COPYABLE(SimpleCallOutBox)
		};
	}
}