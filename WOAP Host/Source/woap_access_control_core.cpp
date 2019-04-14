#include "woap_access_control_core.h"
#include "woap_user.h"
#include "woap_user_group.h"
#include "woap_user_permission.h"
#include "woap_user_login.h"
#include "woap_password_helper.h"
#include "woap_bootstrapper.h"
#include "woap_configuration_manager.h"

using namespace WOAP;

SINGLETON_IMPLEMENTATION(AccessControlCore)

AccessControlCore::AccessControlCore()
{
	for (UserPermission* permission : Bootstrapper::getInstance()->getReportedPermissions())
	{
		permissions.add(new UserPermission(*permission));
	}
	jassert(!permissions.isEmpty()); /* If empty, a new instance of this core was maybe created? */
	
	adminGroup = new UserGroup();
	groups.add(adminGroup);

	for (UserPermission* permission : permissions)
	{
		adminGroup->addPermission(permission);
	}

	adminUser = new User();
	adminUser->setUsername("admin");
	adminUser->setPlainPassword(ConfigurationManager::getInstance()->getAdminPassword());
	adminUser->addToGroup(adminGroup);
	users.add(adminUser);

	if (ConfigurationManager::getInstance()->isAnonymousLoginEnabled())
	{
		anonymousUser = new User();
		anonymousUser->setUsername("anonymous");
		anonymousUser->setPlainPassword("");
		anonymousUser->setIsAnonymous(true);
		anonymousUser->addToGroup(adminGroup);
		users.add(anonymousUser);
	}
	else
	{
		anonymousUser = nullptr;
	}

	ConfigurationManager::getInstance()->addListener(this);
}

AccessControlCore::~AccessControlCore()
{
	ConfigurationManager::getInstance()->removeListener(this);
}

User* AccessControlCore::findUserWithPassword(const String& plainPassword) const
{
	for (User* user : users)
	{
		if (user->isAnonymous())
			continue;

		if (PasswordHelper::hashPassword(plainPassword, user->getSalt()) == user->getPassword())
			return user;
	}

	return nullptr;
}

UserLogin* AccessControlCore::findLoginByLogTag(const String& logTag) const
{
	for (User* user : users)
	{
		const ScopedLock sl2(user->getLogins().getLock());
		for (UserLogin* login : user->getLogins())
		{
			if (login->getLogTag() == logTag && login->isActive())
			{
				return login;
			}
		}
	}


	return nullptr;
}

ValueTree AccessControlCore::getState() const
{
	ValueTree state("ACCESSCONTROLCORE");

	ValueTree adminState("ADMIN");
	for (UserLogin* login : adminUser->getLogins())
	{
		if (login->isActive())
			adminState.addChild(login->getState(), -1, nullptr);
	}
	state.addChild(adminState, -1, nullptr);

	if (anonymousUser != nullptr && ConfigurationManager::getInstance()->isAnonymousLoginEnabled())
	{
		ValueTree anonymousState("ANONYMOUS");
		for (UserLogin* login : anonymousUser->getLogins())
		{
			if (login->isActive())
				anonymousState.addChild(login->getState(), -1, nullptr);
		}
		state.addChild(anonymousState, -1, nullptr);
	}

	return state;
}

void AccessControlCore::setState(const ValueTree& state)
{
	if (!state.hasType("ACCESSCONTROLCORE"))
		return;

	for (ValueTree adminLoginState : state.getChildWithName("ADMIN"))
	{
		UserLogin* login = UserLogin::fromState(adminUser, adminLoginState);
		if (login != nullptr)
			adminUser->addLogin(login);
	}

	if (anonymousUser != nullptr && ConfigurationManager::getInstance()->isAnonymousLoginEnabled())
	{
		for (ValueTree anonymousLoginState : state.getChildWithName("ANONYMOUS"))
		{
			UserLogin* login = UserLogin::fromState(anonymousUser, anonymousLoginState);
			if (login != nullptr)
				anonymousUser->addLogin(login);
		}
	}
}

void AccessControlCore::configurationChanged(const ConfigurationManager::Configuration& oldConfig, const ConfigurationManager::Configuration& newConfig)
{
	if (oldConfig.adminPassword != newConfig.adminPassword)
	{
		adminUser->setPlainPassword(newConfig.adminPassword.toString());
		for (UserLogin* login : adminUser->getLogins())
		{
			login->invalidate();
		}

		if (anonymousUser != nullptr)
		{
			for (UserLogin* login : anonymousUser->getLogins())
			{
				login->invalidate();
			}
		}	
	}
}
