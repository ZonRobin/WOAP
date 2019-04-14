#include "woap_bootstrapper.h"
#include "woap_user_permission.h"

using namespace WOAP;

Bootstrapper* Bootstrapper::instance = nullptr;

Bootstrapper::Bootstrapper() {}
Bootstrapper::~Bootstrapper() {}

Bootstrapper* Bootstrapper::getInstance()
{
	if (instance == nullptr)
	{
		instance = new Bootstrapper();
	}

	return instance;
}

void Bootstrapper::deleteInstance()
{
	ScopedPointer<Bootstrapper> whosBoy(instance);
	instance = nullptr;
}

void Bootstrapper::callInitializers()
{
	/* You should not call initializers twice */
	jassert(!initializersCalled);
	jassert(initializers.size() != 0);
	
	initializersCalled = true;
	initializers.call(&Bootstrapper::Initializer::initialize, *this);
}

const Array<Bootstrapper::Initializer*>& Bootstrapper::getInitializers() const
{
	return initializers.getListeners();
}

void Bootstrapper::addInitializer(Bootstrapper::Initializer* initializer)
{
	/* Initializers already called */
	jassert(!initializersCalled);

	initializers.add(initializer);
}

void Bootstrapper::removeInitializer(Bootstrapper::Initializer* initializer)
{
	initializers.remove(initializer);
}

void Bootstrapper::reportPermissions(const initializer_list<UserPermission*>& permissions)
{
	reportedPermissions.ensureStorageAllocated(reportedPermissions.size() + permissions.size());
	for (UserPermission* permission : permissions)
	{
		reportedPermissions.add(permission);
	}
}

const OwnedArray<UserPermission>& Bootstrapper::getReportedPermissions() const
{
	return reportedPermissions;
}





Bootstrapper::Initializer::Initializer(void(*initializerCallback) (Bootstrapper&))
	: initCallback(initializerCallback)
{
	Bootstrapper::getInstance()->addInitializer(this);
}

Bootstrapper::Initializer::~Initializer() {}

void Bootstrapper::Initializer::initialize(Bootstrapper& bootstrapper)
{
	initCallback(bootstrapper);
}