#include "woap_dca.h"
#include "woap_parameter_bool.h"
#include "woap_parameter_float.h"
#include "woap_dca_controller.h"

using namespace WOAP;

DCA::DCA()
{
	muteParameter = new ParameterBool("dca_mute", "DCA mute", false);
	soloParameter = new ParameterBool("dca_solo", "DCA solo", false);
	gainParameter = new ParameterFloat("dca_gain", "DCA gain", -150.0f, 10.0f, 0);

	muteParameter->addListener(this);
	soloParameter->addListener(this);
	gainParameter->addListener(this);
}

DCA::~DCA()
{
	unassignFromAllControllers();
}

ParameterBool* DCA::getMuteParameter() const
{
	return muteParameter;
}

ParameterBool* DCA::getSoloParameter() const
{
	return soloParameter;
}

ParameterFloat* DCA::getGainParameter() const
{
	return gainParameter;
}

bool DCA::getMuteState() const
{
	return *muteParameter;
}

bool DCA::getSoloState() const
{
	return *soloParameter;
}

float DCA::getGainState() const
{
	return *gainParameter;
}

void DCA::assignToController(DCAController* controller)
{
	if (!assignedControllers.contains(controller))
	{
		assignedControllers.add(controller);
		controller->dcaWasAssigned(this);
	}
}

void DCA::unassignFromController(DCAController* controller)
{
	if (assignedControllers.contains(controller))
	{
		assignedControllers.removeFirstMatchingValue(controller);
		controller->dcaWasUnassigned(this);
	}
}

void DCA::unassignFromAllControllers()
{
	Array<DCAController*, CriticalSection> controllers(assignedControllers);
	assignedControllers.clearQuick();

	for (DCAController* controller : controllers)
	{
		controller->dcaWasUnassigned(this);
	}
}

bool DCA::isAssignedToController(DCAController* controller)
{
	return assignedControllers.contains(controller);
}

const Array<DCAController*, CriticalSection>& DCA::getAssignedControllers() const
{
	return assignedControllers;
}

void DCA::parameterChanged(Parameter* parameter)
{
	const ScopedLock l(assignedControllers.getLock());
	
	if (parameter == gainParameter)
	{
		for (DCAController* controller : assignedControllers)
		{
			controller->gainStateChanged();
		}
	}
	else if (parameter == muteParameter)
	{
		for (DCAController* controller : assignedControllers)
		{
			controller->muteStateChanged();
		}
	}
	else if (parameter == soloParameter)
	{
		for (DCAController* controller : assignedControllers)
		{
			controller->soloStateChanged();
		}
	}
}




MuteStateDCA::MuteStateDCA() {}
MuteStateDCA::~MuteStateDCA() {}

bool MuteStateDCA::getMuteState() const
{
	return *muteParameter && !disabled;
}

bool MuteStateDCA::getSoloState() const
{
	return false;
}

float MuteStateDCA::getGainState() const
{
	return 0;
}

void MuteStateDCA::setDisabled(bool newDisabled)
{
	disabled = newDisabled;
	parameterChanged(muteParameter);
}

void MuteStateDCA::assignToController(DCAController* controller)
{
	if (*muteParameter == false)
	{
		unassignFromAllControllers();
	}

	DCA::assignToController(controller);
	*muteParameter = true;
}

void MuteStateDCA::unassignFromController(DCAController* controller)
{
	DCA::unassignFromController(controller);

	if (assignedControllers.size() == 0)
	{
		*muteParameter = false;
	}
}

void MuteStateDCA::parameterChanged(Parameter*)
{
	if (assignedControllers.size() == 0 && *muteParameter == true)
	{
		*muteParameter = false;
		return;
	}

	const ScopedLock l(assignedControllers.getLock());

	for (DCAController* controller : assignedControllers)
	{
		controller->muteStateChanged();
	}
}


SoloStateDCA::SoloStateDCA(MuteStateDCA* muteDCA) : muteStateDCA(muteDCA) {}
SoloStateDCA::~SoloStateDCA() {}

bool SoloStateDCA::getMuteState() const
{
	return false;
}

bool SoloStateDCA::getSoloState() const
{
	return *soloParameter;
}

float SoloStateDCA::getGainState() const
{
	return 0;
}

void SoloStateDCA::assignToController(DCAController* controller)
{
	if (*soloParameter == false)
	{
		unassignFromAllControllers();
	}

	DCA::assignToController(controller);
	*soloParameter = true;
}

void SoloStateDCA::unassignFromController(DCAController* controller)
{
	DCA::unassignFromController(controller);

	if (assignedControllers.size() == 0)
	{
		*soloParameter = false;
	}
}

void SoloStateDCA::parameterChanged(Parameter*)
{
	if (assignedControllers.size() == 0 && *soloParameter == true)
	{
		*soloParameter = false;
		return;
	}

	muteStateDCA->setDisabled(*soloParameter);

	const ScopedLock l(assignedControllers.getLock());

	for (DCAController* controller : assignedControllers)
	{
		controller->soloStateChanged();
	}
}