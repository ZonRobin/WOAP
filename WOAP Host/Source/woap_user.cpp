#include "woap_user.h"
#include "woap_user_group.h"
#include "woap_user_permission.h"
#include "woap_access_control_core.h"
#include "woap_user_login.h"

using namespace WOAP;

User::User() 
{ 
	
}

User::~User() {}

void User::addToGroup(UserGroup* group)
{
	groups.add(group);
	group->users.add(this);

	groupsChanged();
}

void User::removeFromGroup(UserGroup* group)
{
	groups.removeFirstMatchingValue(group);
	group->users.removeFirstMatchingValue(this);

	groupsChanged();
}

void User::setPlainPassword(const String& newPlainPassword)
{
	salt = PasswordHelper::generateSalt();
	password = PasswordHelper::hashPassword(newPlainPassword, salt);
}

bool User::hasPermissions(const Array<uint32>& permissionIds) const
{
	jassert(permissionIds.size() != 0);

	for (uint32 permId : permissionIds)
	{
		if (!permissionsOwned.contains(permId)) 
		{
			return false;
		}
	}

	return true;
}

void User::groupsChanged()
{
	permissionsOwned.clearQuick();
	permissionsOwned.ensureStorageAllocated(150);

	for (UserGroup* group : groups)
	{
		for (UserPermission* permission : group->getPermissions())
		{
			permissionsOwned.addIfNotAlreadyThere(permission->getId());
		}
	}
}

UserLogin* User::registerLogin()
{
	return logins.add(new UserLogin(this));
}