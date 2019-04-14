#include "woap_dca_controller.h"
#include "woap_dca.h"
#include "woap_track.h"
#include "woap_parameter_bool.h"
#include "woap_parameter_float.h"

using namespace WOAP;


DCAController::DCAController(Track* trackToControll)
	: track(trackToControll)
{
	
}

DCAController::~DCAController()
{
	ignoreAssigns = true;
	for (DCA* dca : assignedDCAs)
	{
		dca->unassignFromController(this);
	}
}

Array<DCA*, CriticalSection>& DCAController::getAssignedDCAs()
{
	return assignedDCAs;
}

void DCAController::dcaWasAssigned(DCA* dca)
{
	assignedDCAs.add(dca);

	if (!ignoreAssigns) 
	{
		muteStateChanged();
		soloStateChanged();
		gainStateChanged();
	}
}

void DCAController::dcaWasUnassigned(DCA* dca)
{
	assignedDCAs.removeFirstMatchingValue(dca);

	if (!ignoreAssigns)
	{
		muteStateChanged();
		soloStateChanged();
		gainStateChanged();
	}
}

void DCAController::muteStateChanged()
{
	bool newMuteState = false;

	{
		const ScopedLock l(assignedDCAs.getLock());
		for (DCA* dca : assignedDCAs)
		{
			newMuteState = newMuteState || dca->getMuteState();
		}
	}

	if (newMuteState != muteState)
	{
		muteState = newMuteState;
		*track->getMuteParameter() = newMuteState;
	}
}

void DCAController::soloStateChanged()
{
	bool newSoloState = false;

	{
		const ScopedLock l(assignedDCAs.getLock());
		for (DCA* dca : assignedDCAs)
		{
			newSoloState = newSoloState || dca->getSoloState();
		}
	}

	if (newSoloState != soloState)
	{
		soloState = newSoloState;
		*track->getSoloParameter() = newSoloState;
	}
}

void DCAController::gainStateChanged()
{
	float newGainState = 0;

	{
		const ScopedLock l(assignedDCAs.getLock());
		for (DCA* dca : assignedDCAs)
		{
			newGainState = newGainState + dca->getGainState();
		}
	}

	if (newGainState != gainState)
	{
		gainState = newGainState;
	}
}