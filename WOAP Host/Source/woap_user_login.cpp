#include "woap_user_login.h"
#include "woap_functions.h"

using namespace WOAP;

/* 30 days */
const int64 UserLogin::LoginExpiration = 1000LL * 60 * 60 * 24 * 30;

UserLogin::UserLogin(User* u)
	: logTag(getRandomString(16)), user(u)
{
	refresh();
}

UserLogin::~UserLogin()
{

}

void UserLogin::refresh()
{
	if (time >= 0)
		time = Time::currentTimeMillis();
}

void UserLogin::invalidate()
{
	time = -1;
}

bool UserLogin::isActive() const
{
	return time + UserLogin::LoginExpiration > Time::currentTimeMillis();
}

ValueTree UserLogin::getState() const
{
	ValueTree state("USERLOGIN");

	state.setProperty("time", var(time), nullptr);
	state.setProperty("logTag", var(logTag), nullptr);

	return state;
}

void UserLogin::setState(const ValueTree& state)
{
	if (!state.hasType("USERLOGIN"))
		return;

	logTag = StateHelper::getStringProperty(state, "logTag");
	time = StateHelper::getInt64Property(state, "time");
}

UserLogin* UserLogin::fromState(User* user, const ValueTree& state)
{
	if (!state.hasType("USERLOGIN"))
		return nullptr;

	UserLogin* login = new UserLogin(user);
	login->setState(state);

	return login;
}
