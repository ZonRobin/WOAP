#include "woap_functions.h"

#ifdef JUCE_WINDOWS
#include <shlobj.h>
#endif

using namespace WOAP;

StringArray WOAP::varToStringArray(const var& value)
{
	StringArray output;
	if (!value.isArray())
		return output;

	for (var& item : *value.getArray())
	{
		if (item.isString()) 
			output.add(item.toString());
	}

	return output;
}

bool WOAP::intToBool(int value)
{
	return value > 0;
}

int WOAP::boolToInt(bool value)
{
	return value ? 1 : 0;
}

template <>
bool WOAP::isVarOfType<float>(const var& value)
{
	return value.isDouble();
}

template <>
bool WOAP::isVarOfType<double>(const var& value)
{
	return value.isDouble();
}

template <>
bool WOAP::isVarOfType<String>(const var& value)
{
	return value.isString();
}

template <>
bool WOAP::isVarOfType<int>(const var& value)
{
	return value.isInt();
}

String WOAP::getRandomString(int length)
{
	jassert(length >= 0);

	const int size = length / 2;

	Random r;
	HeapBlock<uint8> chars(sizeof(uint8) * size);

	for (int i = 0; i < size; ++i)
	{
		chars[i] = (uint8)(r.nextInt(256));
	}

	return String::toHexString(chars.getData(), size, 0);
}

Uuid WOAP::uuidFromDashedString(const String& dashed)
{
	const String nonDashed = dashed.removeCharacters("-");
	
	if (nonDashed.length() != 32)
		return Uuid();
	else
		return Uuid(nonDashed);
}

void WOAP::executeCommand(const String& command)
{
#ifdef JUCE_WINDOWS
	const String cmd = L"/C " + command;
	ShellExecuteW(0, L"open", L"cmd.exe", cmd.toWideCharPointer(), 0, SW_HIDE);
#else
	ignoreUnused(command);
	jassertfalse;
#endif
}
