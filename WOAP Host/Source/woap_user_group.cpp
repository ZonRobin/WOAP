#include "woap_user_group.h"
#include "woap_user_permission.h"
#include "woap_user.h"

using namespace WOAP;

UserGroup::UserGroup(const String& nm, const String& desc)
	: name(nm), description(desc)
{

}

UserGroup::UserGroup(const Uuid& i, const String& nm, const String& desc)
	: Identifiable(i), name(nm), description(desc)
{

}

UserGroup::~UserGroup()
{

}

void UserGroup::addPermission(UserPermission* newPermission)
{
	permissions.add(newPermission);
	for (User* user : users)
	{
		user->groupsChanged();
	}
}

void UserGroup::removePermission(UserPermission* permissionToRemove)
{
	permissions.removeAllInstancesOf(permissionToRemove);
	for (User* user : users)
	{
		user->groupsChanged();
	}
}

void UserGroup::addUser(User* userToAdd)
{
	users.add(userToAdd);
	userToAdd->groups.add(this);
}

void UserGroup::removeUser(User* userToRemove)
{
	users.removeFirstMatchingValue(userToRemove);
	userToRemove->groups.removeFirstMatchingValue(this);
}