#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define INSTANCEOF(I, C) (dynamic_cast<C*>(I) != nullptr)

#define ASSERT_ONLY_MESSAGE_THREAD jassert(MessageManager::getInstance()->isThisTheMessageThread())


namespace WOAP
{
	enum NumChannels
	{
		None = 0,
		Mono = 1,
		Stereo = 2
	};

	template <class ListenerType>
	using LockedListenerList = ListenerList<ListenerType, Array<ListenerType*, CriticalSection>>;

	const double GainSmoothingTimeSecs = 0.002;

	class AudioPlaybackInfoListener
	{
	public:
		virtual void playbackAboutToStart(double newSampleRate, int newBufferSizeSamples) = 0;
		virtual void playbackAboutToStop() {}
	};

	template <typename Type>
	String arrayToString(const Array<Type>& arr, const String& glue = String())
	{
		String output;
		for (int i = 0; i < arr.size(); ++i)
		{
			if (i == arr.size() - 1)
				output += String(arr[i]);
			else
				output += String(arr[i]) + glue;
		}

		return output;
	}

	template <typename IntType = int>
	IntType getIntFromStringEnd(const String& str)
	{
		const int index = str.lastIndexOfChar(L' ');
		if (index == -1)
			return (IntType)-1;

		const String substr = str.substring(index + 1);
		if (!substr.containsOnly("1234567890"))
			return (IntType)-1;

		return CharacterFunctions::getIntValue<IntType>(substr.toUTF8());
	}

	template <typename Type>
	int getFirstMissingNumber(const Array<Type>& values, Type startValue = 0)
	{
		if (values.isEmpty())
			return startValue;

		Type lastValue = startValue - (Type)1;
		for (Type value : values)
		{				
			if (value - lastValue > (Type)1)
				return lastValue + (Type)1;
			else if (value >= startValue)
				lastValue = value;
		}

		return jmax(values.getLast() + (Type)1, startValue);
	}
}
