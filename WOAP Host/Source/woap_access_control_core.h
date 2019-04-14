#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"
#include "woap_stateful.h"
#include "woap_configuration_manager.h"

namespace WOAP
{
	class User;
	class UserGroup;
	class UserLogin;
	class UserPermission;

	class AccessControlCore : public Stateful, public ConfigurationManager::Listener
	{
	public:
		User* getAnonymousUserOrNull() const { return anonymousUser; }
		User* findUserWithPassword(const String& plainPassword) const;
		UserLogin* findLoginByLogTag(const String& logTag) const;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;

		virtual void configurationChanged(const ConfigurationManager::Configuration& oldConfig, const ConfigurationManager::Configuration& newConfig) override;

	protected:
		OwnedArray<User> users;
		OwnedArray<UserGroup> groups;
		OwnedArray<UserPermission> permissions;

		User* anonymousUser;
		User* adminUser;
		UserGroup* adminGroup;

	private:
		SINGLETON(AccessControlCore)
	};
}