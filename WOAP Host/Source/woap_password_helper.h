#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_functions.h"

namespace WOAP
{
	class PasswordHelper
	{
	public:
		static String generateSalt()
		{
			return getRandomString(32);
		}

		static String hashPassword(const String& plainPassword, const String& salt)
		{
			return Whirlpool((plainPassword + salt).toUTF8()).toHexString();
		}

	private:
		PasswordHelper() {}
		~PasswordHelper() {}
	};
}