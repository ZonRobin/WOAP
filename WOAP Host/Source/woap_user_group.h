#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_identifiable.h"

namespace WOAP
{
	class UserPermission;
	class User;

	class UserGroup : public Identifiable
	{
	public:
		UserGroup(const String& name = "", const String& description = "");
		UserGroup(const Uuid& id, const String& name, const String& description);
		virtual ~UserGroup();

		String getName() const { return name; }
		String getDescription() const { return description; }

		const Array<UserPermission*>& getPermissions() const { return permissions; }
		void addPermission(UserPermission* newPermission);
		void removePermission(UserPermission* permissionToRemove);

		void addUser(User* userToAdd);
		void removeUser(User* userToRemove);

	protected:
		String name;
		String description;
		Array<User*> users;
		Array<UserPermission*> permissions;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UserGroup)
		friend class User;
	};
}