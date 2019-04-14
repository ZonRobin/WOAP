#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_stateful.h"

namespace WOAP
{
	class User;

	class UserLogin : public Stateful
	{
	public:
		static const int64 LoginExpiration;

		UserLogin(User* user);
		virtual ~UserLogin();

		void refresh();
		void invalidate();

		bool isValid() const { return time >= 0; }

		const String& getLogTag() const { return logTag; }
		int64 getTime() const { return time; }
		User* getUser() const { return user; }

		bool isActive() const;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static UserLogin* fromState(User* user, const ValueTree& state);

	protected:
		User* user;
		String logTag;
		int64 time = 0;
	};
}