#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	class UserPermission
	{
	public:
		UserPermission(uint32 id, const String& name = "", const String& description = "");
		UserPermission(const UserPermission& other);

		virtual ~UserPermission();

		uint32 getId() const;
		String getName() const;
		String getDescription() const;

	protected:
		uint32 id;
		String name;
		String description;

	private:
		JUCE_LEAK_DETECTOR(UserPermission)
	};
}
