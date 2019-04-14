#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_password_helper.h"
#include "woap_identifiable.h"

using std::initializer_list;

namespace WOAP
{
	class UserGroup;
	class UserLogin;

	class User : public Identifiable
	{
	public:
		User();
		virtual ~User();

		String getUsername() const { return username; }
		String getPassword() const { return password; }
		String getSalt() const { return salt; }
		const OwnedArray<UserLogin, CriticalSection>& getLogins() const { return logins; }
		bool isActive() const { return active; }
		bool isAnonymous() const { return anonymous; }
		const Array<UserGroup*>& getGroups() const { return groups; }
		void addToGroup(UserGroup* group);
		void removeFromGroup(UserGroup* group);

		void setIsAnonymous(bool anonym) { anonymous = anonym; }
		void setUsername(const String& newUsername) { username = newUsername; }
		void setPlainPassword(const String& newPlainPassword);
		bool hasPermission(uint32 permId) const { return permissionsOwned.contains(permId); }
		bool hasPermissions(const Array<uint32>& permissionIds) const;
		void groupsChanged();

		UserLogin* registerLogin();
		void addLogin(UserLogin* login) { logins.add(login); }

	protected:
		String username;
		String password;
		String salt;
		bool active = true;
		bool anonymous = false;
		Array<UserGroup*> groups;
		OwnedArray<UserLogin, CriticalSection> logins;
		Array<uint32> permissionsOwned; /* Cached permission ids */

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(User)
		friend class UserGroup;
	};
}
