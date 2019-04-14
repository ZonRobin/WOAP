#include "woap_user_permission.h"

using namespace WOAP;

UserPermission::UserPermission(uint32 i, const String& n, const String& desc)
	: id(i), name(n), description(desc)
{

}


UserPermission::UserPermission(const UserPermission& other)
	: id(other.id), name(other.name), description(other.description)
{

}

UserPermission::~UserPermission()
{

}

uint32 UserPermission::getId() const
{
	return id;
}

String UserPermission::getName() const
{
	return name;
}

String UserPermission::getDescription() const
{
	return description;
}