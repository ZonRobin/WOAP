#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	template <typename Type>
	class FastFIFO
	{
	public:
		FastFIFO()
		{
			numAllocated = 1;
			elements.allocate(1, true);
		}

		virtual ~FastFIFO() 
		{
		
		}

		void resize(uint32 numItems)
		{
			const SpinLock::ScopedLockType sl(lock);

			if (numItems > numAllocated)
			{
				numAllocated = numItems * 2;
				writeIndex = 0;
				readIndex = 0;

				elements.allocate(numAllocated, true);
			}
		}

		void push(Type item)
		{
			const SpinLock::ScopedLockType sl(lock);
			
			elements[writeIndex] = item;
			if (writeIndex >= numAllocated)
				writeIndex = 0;
			else
				++writeIndex;
		}

		Type pop()
		{
			if (writeIndex <= readIndex)
				return Type();

			const SpinLock::ScopedLockType sl(lock);
			if (writeIndex <= readIndex)
				return Type();

			Type item(elements[readIndex]);
			elements[readIndex] = Type();

			if (readIndex >= numAllocated)
				readIndex = 0;
			else
				++readIndex;

			return item;
		}

		void clear()
		{
			const SpinLock::ScopedLockType sl(lock);
			
			for (uint32 i = 0; i < writeIndex; ++i)
				elements[i] = Type();

			writeIndex = 0;
			readIndex = 0;
		}

	protected:
		SpinLock lock;

		HeapBlock<Type> elements;
		uint32 numAllocated = 0;

		uint32 writeIndex = 0;
		uint32 readIndex = 0;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastFIFO)
	};
}