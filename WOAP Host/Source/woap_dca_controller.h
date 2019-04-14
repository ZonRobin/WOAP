#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class Track;
	class DCA;

	class DCAController
	{
	public:
		DCAController(Track* trackToControll);
		virtual ~DCAController();

		Array<DCA*, CriticalSection>& getAssignedDCAs();

		void dcaWasAssigned(DCA* dca);
		void dcaWasUnassigned(DCA* dca);

		void muteStateChanged();
		void soloStateChanged();
		void gainStateChanged();

	protected:
		Track* track;
		Array<DCA*, CriticalSection> assignedDCAs;

		bool muteState = false;
		bool soloState = false;
		float gainState = 0;

		bool ignoreAssigns = false;
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DCAController)
	};
}
