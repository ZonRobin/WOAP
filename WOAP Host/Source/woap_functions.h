#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	template <typename T>
	bool isVarOfType(const var& value)
	{
		/* You should define new expicit template method for your type */
		jassertfalse;
		return false;
	}

	template <> bool isVarOfType<float>(const var& value);
	template <> bool isVarOfType<double>(const var& value);
	template <> bool isVarOfType<String>(const var& value);
	template <> bool isVarOfType<int>(const var& value);

	template <typename T>
	Array<T> varToArray(const var& value)
	{
		Array<T> output;
		if (!value.isArray())
			return output;

		for (var& item : value.getArray())
		{
			if (isVarOfType<T>(item))
				output.add(static_cast<T>(item));
		}

		return output;
	}

	StringArray varToStringArray(const var& value);

	bool intToBool(int value);
	int boolToInt(bool value);

	String getRandomString(int length);

	Uuid uuidFromDashedString(const String& dashed);

	inline int16 doubleToInt16Sample(double value)
	{
		if (value >= 1.0)
			return 32767;
		else if (value <= -1.0)
			return -32768;
		else
			return (int16)floor(value * 32768);
	}

	inline int16 floatToInt16Sample(float value)
	{
		if (value >= 1.0f)
			return 32767;
		else if (value <= -1.0f)
			return -32768;
		else
			return (int16)floorf(value * 32768);
	}

	inline Array<int> bigIntToArray(const BigInteger& mrBig)
	{
		const int numItems = mrBig.getHighestBit() + 1;

		Array<int> out;
		out.ensureStorageAllocated(numItems);

		for (int i = 0; i < numItems; ++i)
		{
			if (mrBig[i])
				out.add(i);
		}

		return out;
	}

	void executeCommand(const String& command);
}