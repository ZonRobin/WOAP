#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	template <typename Type>
	class AudioBufferExtended : protected AudioBuffer<Type>
	{
	public:
		/** Creates an empty buffer with 0 channels and 0 length. */
		AudioBufferExtended() noexcept : AudioBuffer<Type>()
		{
		}

		/** Creates a buffer with a specified number of channels and samples.

		The contents of the buffer will initially be undefined, so use clear() to
		set all the samples to zero.

		The buffer will allocate its memory internally, and this will be released
		when the buffer is deleted. If the memory can't be allocated, this will
		throw a std::bad_alloc exception.
		*/
		AudioBufferExtended(int numChannelsToAllocate, int numCurrentSamplesToAllocate, int numHistorySamplesToAllocate = 0) noexcept : 
			numCurrentSamples(numCurrentSamplesToAllocate), 
			numHistorySamples(numHistorySamplesToAllocate), 
			AudioBuffer<Type>(numChannelsToAllocate, numCurrentSamplesToAllocate + numHistorySamplesToAllocate)
		{
			
		}

		/** Creates a buffer using a pre-allocated block of memory.

		Note that if the buffer is resized or its number of channels is changed, it
		will re-allocate memory internally and copy the existing data to this new area,
		so it will then stop directly addressing this memory.

		@param dataToReferTo    a pre-allocated array containing pointers to the data
		for each channel that should be used by this buffer. The
		buffer will only refer to this memory, it won't try to delete
		it when the buffer is deleted or resized.
		@param numChannelsToUse the number of channels to use - this must correspond to the
		number of elements in the array passed in
		@param numSamples       the number of samples to use - this must correspond to the
		size of the arrays passed in
		*/
		AudioBufferExtended(Type* const* dataToReferTo, int numChannelsToUse, int numSamples, int numSrcHistorySamples = 0) noexcept : 
			numCurrentSamples(numSamples),
			numHistorySamples(numSrcHistorySamples),
			AudioBuffer<Type>(dataToReferTo, numChannelsToUse, numSamples)
		{
			
		}

		/** Creates a buffer using a pre-allocated block of memory.

		Note that if the buffer is resized or its number of channels is changed, it
		will re-allocate memory internally and copy the existing data to this new area,
		so it will then stop directly addressing this memory.

		@param dataToReferTo    a pre-allocated array containing pointers to the data
		for each channel that should be used by this buffer. The
		buffer will only refer to this memory, it won't try to delete
		it when the buffer is deleted or resized.
		@param numChannelsToUse the number of channels to use - this must correspond to the
		number of elements in the array passed in
		@param startSample      the offset within the arrays at which the data begins
		@param numSamples       the number of samples to use - this must correspond to the
		size of the arrays passed in
		*/
		AudioBufferExtended(Type* const* dataToReferTo, int numChannelsToUse, int startSample, int numSamples, int numSrcHistorySamples = 0) noexcept :
			numCurrentSamples(numSamples),
			numHistorySamples(numSrcHistorySamples),
			AudioBuffer<Type>(dataToReferTo, numChannelsToUse, startSample, numSamples)
		{

		}

		/** Copies another buffer.

		This buffer will make its own copy of the other's data, unless the buffer was created
		using an external data buffer, in which case boths buffers will just point to the same
		shared block of data.
		*/
		AudioBufferExtended(const AudioBuffer& other) noexcept : AudioBuffer<Type>(other),
			numCurrentSamples(other.getNumSamples()),
		{

		}

		/** Copies another buffer.

		This buffer will make its own copy of the other's data, unless the buffer was created
		using an external data buffer, in which case boths buffers will just point to the same
		shared block of data.
		*/
		AudioBufferExtended(const AudioBufferExtended& other) noexcept : 
			numHistorySamples(other.numHistorySamples),
			numCurrentSamples(other.numCurrentSamples),
			AudioBuffer<Type>(other)
		{

		}

		/** Destructor.
		This will free any memory allocated by the buffer.
		*/
		~AudioBufferExtended() noexcept {}

#if JUCE_COMPILER_SUPPORTS_MOVE_SEMANTICS
		/** Move constructor */
		AudioBufferExtended(AudioBuffer&& other) noexcept : AudioBuffer<Type>(other)
		{
			
		}

		/** Move constructor */
		AudioBufferExtended(AudioBufferExtended&& other) noexcept :
			numHistorySamples(other.numHistorySamples),
			numCurrentSamples(other.numCurrentSamples),
			AudioBuffer<Type>(other)
		{

		}
#endif

		AudioBuffer<Type>& toJuceBuffer()
		{
			/* This should not be used while there's history in the buffer */
			jassert(numHistorySamples == 0);

			return static_cast<AudioBuffer<Type>&>(*this);
		}

		const AudioBuffer<Type>& toJuceConstBuffer() const
		{
			/* This should not be used while there's history in the buffer */
			jassert(numHistorySamples == 0);

			return static_cast<const AudioBuffer<Type>&>(*this);
		}

		void copyFrom(int destChannel, int destStartSample, const Type* source, int numSamples, Type gain = (Type)1.0)
		{
			jassert(isPositiveAndBelow(destChannel, getNumChannels()));
			jassert(destStartSample >= -numHistorySamples && (destStartSample + numSamples) <= getNumCurrentSamples());
			jassert(source != nullptr);

			if (numSamples <= 0)
				return;

			if (gain == 0)
			{
				FloatVectorOperations::clear(getWritePointer(destChannel, destStartSample), numSamples);
			}
			else
			{
				if (gain == (Type)1.0)
					FloatVectorOperations::copy(getWritePointer(destChannel, destStartSample), source, numSamples);
				else
					FloatVectorOperations::copyWithMultiply(getWritePointer(destChannel, destStartSample), source, gain, numSamples);
			}
		}

		template <typename OtherType>
		void copyFrom(int destChannel, int destStartSample, const OtherType* source, int numSamples, OtherType gain = (OtherType)1.0)
		{			
			const int endSample = destStartSample + numSamples + numHistorySamples;

			jassert(isPositiveAndBelow(destChannel, getNumChannels()));
			jassert(destStartSample >= -numHistorySamples && endSample <= getNumSamples());
			jassert(source != nullptr);

			if (numSamples <= 0)
			{
				return;
			}

			if (gain != (OtherType)1.0)
			{
				if (gain == (OtherType)0.0)
				{
					clear(destChannel, destStartSample, numSamples);
				}
				else
				{
					Type* const dest = getWritePointer(destChannel, -numHistorySamples);
					const int offset = destStartSample + numHistorySamples;

					for (int i = offset; i < endSample; ++i)
						dest[i] = static_cast<Type>(source[i - offset]) * gain;
				}
			}
			else
			{
				Type* const dest = getWritePointer(destChannel, -numHistorySamples);
				const int offset = destStartSample + numHistorySamples;

				for (int i = offset; i < endSample; ++i)
					dest[i] = static_cast<Type>(source[i - offset]);
			}
		}
		
		template <typename OtherType>
		void addFrom(int destChannel, int destStartSample, const OtherType* source, int numSamples, float gainToApplyToSource = 1.0f)
		{
			jassert(isPositiveAndBelow(destChannel, getNumChannels()));
			jassert(destStartSample >= 0 && destStartSample + numSamples <= getNumSamples());
			jassert(source != nullptr);
			
			if (numSamples <= 0)
			{
				return;
			}
			
			Type* dest = getWritePointer(destChannel, -numHistorySamples);

			const int endSample = destStartSample + numSamples + numHistorySamples;
			const int offset = destStartSample + numHistorySamples;

			if (hasBeenCleared())
			{
				if (gainToApplyToSource == 1.0f)
				{
					for (int i = offset; i < endSample; ++i)
						dest[i] = static_cast<Type> (source[i - offset]);
				}
				else
				{
					for (int i = offset; i < endSample; ++i)
						dest[i] = static_cast<Type> (source[i - offset]) * gainToApplyToSource;
				}
			}
			else
			{
				if (gainToApplyToSource == 1.0f)
				{
					for (int i = offset; i < endSample; ++i)
						dest[i] = dest[i] + static_cast<Type> (source[i - offset]);
				}
				else
				{
					for (int i = offset; i < endSample; ++i)
						dest[i] = dest[i] + static_cast<Type> (source[i - offset]) * gainToApplyToSource;
				}
			}
		}

		void addFrom(int destChannel, int destStartSample, const Type* source, int numSamples, float gainToApplyToSource = 1.0f)
		{
			AudioBuffer<Type>::addFrom(destChannel, destStartSample + numHistorySamples, source, numSamples, gainToApplyToSource);
		}

		template <typename OtherType>
		void addFrom(int destSample,
			const AudioBufferExtended<OtherType>& source,
			int sourceStartSample,
			int numSamples) noexcept
		{			
			jassert(numSamples <= source.getNumSamples() && numSamples <= getNumSamples());

			const int destNumChans = getNumChannels();

			if (destNumChans != source.getNumChannels())
			{
				switch (destNumChans)
				{
				case 1:
				{
					addFrom(0, destSample, source.getReadPointer(0, sourceStartSample), numSamples, (Type)0.5);
					addFrom(0, destSample, source.getReadPointer(1, sourceStartSample), numSamples, (Type)0.5);
					return;
				}
				case 2:
				{
					addFrom(0, destSample, source.getReadPointer(0, sourceStartSample), numSamples);
					addFrom(1, destSample, source.getReadPointer(0, sourceStartSample), numSamples);
					return;
				}
				default:
					jassertfalse;
					return;
				}
			}
			else
			{
				for (int channel = 0; channel < destNumChans; ++channel)
				{
					addFrom(channel, destSample, source.getReadPointer(channel, sourceStartSample), numSamples);
				}
			}
		}

		void clearChannel(int channelNumber)
		{
			jassert(isPositiveAndBelow(channelNumber, getNumChannels()));

			FloatVectorOperations::clear(getWritePointer(channelNumber), getNumSamples());
		}


		int getNumChannels() const noexcept { return AudioBuffer<Type>::getNumChannels(); }

		int getNumCurrentSamples() const noexcept { return numCurrentSamples; }

		int getNumHistorySamples() const noexcept { return numHistorySamples; }

		const Type* getReadPointer(int channelNumber) const noexcept
		{
			return AudioBuffer<Type>::getReadPointer(channelNumber, numHistorySamples);
		}

		const Type* getReadPointer(int channelNumber, int sampleIndex) const noexcept
		{
			return AudioBuffer<Type>::getReadPointer(channelNumber, sampleIndex + numHistorySamples);
		}

		Type* getWritePointer(int channelNumber) noexcept
		{
			return AudioBuffer<Type>::getWritePointer(channelNumber, numHistorySamples);
		}

		Type* getWritePointer(int channelNumber, int sampleIndex) noexcept
		{
			return AudioBuffer<Type>::getWritePointer(channelNumber, sampleIndex + numHistorySamples);
		}

		/*
			After this resize the history data are kept but current data may be wrong.
		*/
		void setSize(int newNumChannels,
			int numNewCurrentSamples,
			int numNewHistorySamples) noexcept
		{
			if (numNewHistorySamples != numHistorySamples)
			{
				AudioBufferExtended<Type> oldData(getNumChannels(), numHistorySamples + numCurrentSamples);
				oldData.makeCopyOfWithHistory(*this);

				const int numOldHistorySamples = numHistorySamples;
				const int numOldCurrentSamples = numCurrentSamples;
				numHistorySamples = numNewHistorySamples;
				numCurrentSamples = numNewCurrentSamples;
				
				AudioBuffer<Type>::setSize(newNumChannels, numNewCurrentSamples + numNewHistorySamples, false, true, true);
			
				const int numEmptyHistory = jmax(numNewHistorySamples - numOldHistorySamples, 0);
				clear(-numHistorySamples, numEmptyHistory);

				const int numEmptyCurrent = jmax(numNewCurrentSamples - numOldCurrentSamples, 0);
				clear(numCurrentSamples - numEmptyCurrent, numEmptyCurrent);

				const int newHistoryStart = numEmptyHistory - numHistorySamples;
				const int newCurrentEnd = numCurrentSamples - numEmptyCurrent;
				const int newLength = newCurrentEnd - newHistoryStart;
				const int numChannelsToCopy = jmin(newNumChannels, oldData.getNumChannels());

				if (newLength > 0)
				{
					for (int i = 0; i < numChannelsToCopy; ++i)
					{
						copyFrom(i, newHistoryStart, oldData.getReadPointer(i, -jmin(numOldHistorySamples, numNewHistorySamples)), newLength);
					}
				}
			}
			else
			{
				numHistorySamples = numNewHistorySamples;
				numCurrentSamples = numNewCurrentSamples;

				AudioBuffer<Type>::setSize(newNumChannels, numNewCurrentSamples + numNewHistorySamples, false, true, true);
			}
		}

		void setSize(int newNumChannels,
			int newNumCurrentSamples,
			bool clearExtraSpace = false) noexcept
		{
			numCurrentSamples = newNumCurrentSamples;

			AudioBuffer<Type>::setSize(newNumChannels, newNumCurrentSamples + numHistorySamples, false, clearExtraSpace, true);
		}

		void setDataToReferTo(Type** dataToReferTo,
			const int newNumChannels,
			const int newNumCurrentSamples,
			const int newNumHistorySamples = 0) noexcept
		{
			numHistorySamples = newNumHistorySamples;
			numCurrentSamples = newNumCurrentSamples;

			AudioBuffer<Type>::setDataToReferTo(dataToReferTo, newNumChannels, newNumCurrentSamples + newNumHistorySamples);
		}

		void makeCopyOf(const AudioBufferExtended<Type>& other)
		{
			const int numChann = other.getNumChannels();
			setSize(numChann, numCurrentSamples);

			if (other.hasBeenCleared())
			{
				getWritePointer(0);
				clear();
			}
			else
			{
				for (int chann = 0; chann < numChann; ++chann)
				{
					copyFrom(chann, 0, other.getReadPointer(chann), numCurrentSamples);
				}
			}
		}

		template <typename OtherType>
		void makeCopyOf(const AudioBufferExtended<OtherType>& other)
		{
			numCurrentSamples = other.getNumCurrentSamples();
			const int numChann = other.getNumChannels();

			setSize(numChann, numCurrentSamples);

			if (other.hasBeenCleared())
			{
				getWritePointer(0);
				clear();
			}
			else
			{
				for (int chan = 0; chan < numChann; ++chan)
				{
					Type* const dest = getWritePointer(chan);
					const OtherType* const src = other.getReadPointer(chan);

					for (int i = 0; i < numCurrentSamples; ++i)
						dest[i] = static_cast<Type> (src[i]);
				}
			}
		}

		template <typename OtherType>
		void makeCopyOf(AudioBuffer<OtherType>& other)
		{
			numCurrentSamples = other.getNumSamples();
			const int numChann = other.getNumChannels();

			setSize(numChann, numCurrentSamples);

			if (other.hasBeenCleared())
			{
				getWritePointer(0);
				clear();
			}
			else
			{
				for (int chan = 0; chan < numChann; ++chan)
				{
					Type* const dest = getWritePointer(chan);
					const OtherType* const src = other.getReadPointer(chan);

					for (int i = 0; i < numCurrentSamples; ++i)
						dest[i] = static_cast<Type> (src[i]);
				}
			}
		}

		template <typename OtherType>
		void makeCopyOfWithHistory(const AudioBufferExtended<OtherType>& other)
		{
			numCurrentSamples = other.getNumCurrentSamples();
			numHistorySamples = other.getNumHistorySamples();
			const int numChann = other.getNumChannels();

			setSize(numChann, numCurrentSamples, numHistorySamples);

			if (other.hasBeenCleared())
			{
				clearWithHistory();
			}
			else
			{
				const int length = numCurrentSamples + numHistorySamples;

				for (int chan = 0; chan < numChann; ++chan)
				{
					Type* const dest = getWritePointer(chan, -numHistorySamples);
					const OtherType* const src = other.getReadPointer(chan, -numHistorySamples);

					for (int i = 0; i < length; ++i)
						dest[i] = static_cast<Type> (src[i]);
				}
			}
		}

		void pushToHistory(const AudioBufferExtended<Type>& other)
		{
			if (numHistorySamples == 0)
				return;

			const int numChanns = getNumChannels();
			const int numSamplesToCopy = jmin(numHistorySamples, other.numCurrentSamples);
			const int numChannelsToCopy = jmin(numChanns, other.getNumChannels());

			for (int i = 0; i < numChanns; ++i)
			{
				memmove(
					getWritePointer(i, -numHistorySamples), 
					getReadPointer(i, -numHistorySamples + numSamplesToCopy), 
					(size_t)(numHistorySamples - numSamplesToCopy) * sizeof(Type)
				);
			}			

			for (int chan = 0; chan < numChannelsToCopy; ++chan)
			{
				copyFrom(chan, -numSamplesToCopy, other.getReadPointer(chan, other.numCurrentSamples - numSamplesToCopy), numSamplesToCopy);
			}
		}

		void clearHistory() noexcept
		{
			AudioBuffer<Type>::clear(0, numHistorySamples);
		}

		void clearWithHistory() noexcept
		{
			AudioBuffer<Type>::clear();
		}

		void clear() noexcept
		{
			AudioBuffer<Type>::clear(numHistorySamples, numCurrentSamples);
		}

		void clear(int startSample,
			int numSamples) noexcept
		{
			AudioBuffer<Type>::clear(startSample + numHistorySamples, numSamples);
		}

		void clear(int channel,
			int startSample,
			int numSamples) noexcept
		{
			AudioBuffer<Type>::clear(channel, startSample + numHistorySamples, numSamples);
		}

		bool hasBeenCleared() const noexcept 
		{ 
			return AudioBuffer<Type>::hasBeenCleared(); 
		}

		Type getSample(int channel, int sampleIndex) const noexcept
		{
			return AudioBuffer<Type>::getSample(channel, sampleIndex + numHistorySamples);
		}

		void setSample(int destChannel, int destSample, Type newValue) noexcept
		{
			AudioBuffer<Type>::setSample(destChannel, destSample + numHistorySamples, newValue);
		}

		void addSample(int destChannel, int destSample, Type valueToAdd) noexcept
		{
			AudioBuffer<Type>::addSample(destChannel, destSample + numHistorySamples, valueToAdd);
		}

		void applyGain(int channel,
			int startSample,
			int numSamples,
			Type gain) noexcept
		{
			AudioBuffer<Type>::applyGain(channel, startSample + numHistorySamples, numSamples, gain);
		}

		void applyGain(int startSample,
			int numSamples,
			Type gain) noexcept
		{
			AudioBuffer<Type>::applyGain(startSample + numHistorySamples, numSamples, gain);
		}

		void applyGain(Type gain) noexcept
		{
			AudioBuffer<Type>::applyGain(numHistorySamples, numCurrentSamples, gain);
		}

		void applyGainRamp(int channel,
			int startSample,
			int numSamples,
			Type startGain,
			Type endGain) noexcept
		{
			AudioBuffer<Type>::applyGainRamp(channel, startSample + numHistorySamples, numSamples, startGain, endGain);
		}

		void applyGainRamp(int startSample,
			int numSamples,
			Type startGain,
			Type endGain) noexcept
		{
			AudioBuffer<Type>::applyGainRamp(startSample + numHistorySamples, numSamples, startGain, endGain);
		}

		void addFrom(int destChannel,
			int destStartSample,
			const AudioBufferExtended& source,
			int sourceChannel,
			int sourceStartSample,
			int numSamples,
			Type gainToApplyToSource = (Type)1) noexcept
		{
			AudioBuffer<Type>::addFrom(destChannel, destStartSample + numHistorySamples, source, sourceChannel, sourceStartSample + source.numHistorySamples,  numSamples, gainToApplyToSource);
		}

		void copyFrom(int destChannel,
			int destStartSample,
			const AudioBufferExtended& source,
			int sourceChannel,
			int sourceStartSample,
			int numSamples) noexcept
		{
			AudioBuffer<Type>::copyFrom(destChannel, destStartSample + numHistorySamples, source, sourceChannel, sourceStartSample + source.numHistorySamples, numSamples);
		}

		void copyFrom(int destChannel,
			int destStartSample,
			const AudioBufferExtended& source,
			int sourceChannel,
			int sourceStartSample,
			int numSamples,
			Type gain) noexcept
		{
			jassert(&source != this || sourceChannel != destChannel);
			jassert(isPositiveAndBelow(destChannel, numChannels));
			jassert(destStartSample >= 0 && destStartSample + numSamples <= size);
			jassert(isPositiveAndBelow(sourceChannel, source.numChannels));
			jassert(sourceStartSample >= 0 && sourceStartSample + numSamples <= source.size);

			if (numSamples <= 0)
				return;

			if (source.isClear || gain == 0)
			{
				if (!isClear)
					FloatVectorOperations::clear(getWritePointer(destChannel, destStartSample), numSamples);
			}
			else
			{
				isClear = false;
				if (gain == (Type)1.0)
				{
					FloatVectorOperations::copy(getWritePointer(destChannel, destStartSample),
						source.getReadPointer(sourceChannel, sourceStartSample), numSamples);
				}
				else
				{
					FloatVectorOperations::copyWithMultiply(getWritePointer(destChannel, destStartSample),
						source.getReadPointer(sourceChannel, sourceStartSample), gain, numSamples);
				}
			}
		}

		void copyFrom(int destSample,
			const AudioBufferExtended& source,
			int sourceStartSample,
			int numSamples) noexcept
		{
			jassert(numSamples <= source.getNumSamples() && numSamples <= getNumSamples());

			const int destNumChans = getNumChannels();

			if (destNumChans != source.getNumChannels())
			{
				switch (destNumChans)
				{
				case 1:
				{
					copyFrom(0, destSample, source.getReadPointer(0, sourceStartSample), numSamples, (Type)0.5);
					copyFrom(0, destSample, source.getReadPointer(1, sourceStartSample), numSamples, (Type)0.5);
					return;
				}
				case 2:
				{
					copyFrom(0, destSample, source.getReadPointer(0, sourceStartSample), numSamples);
					copyFrom(1, destSample, source.getReadPointer(0, sourceStartSample), numSamples);
					return;
				}
				default:
					jassertfalse;
					return;
				}
			}
			else
			{
				for (int channel = 0; channel < destNumChans; ++channel)
				{
					copyFrom(channel, destSample, source.getReadPointer(channel, sourceStartSample), numSamples);
				}
			}
		}
		
		Range<Type> findMinMax(int channel,
			int startSample,
			int numSamples) const noexcept
		{
			return AudioBuffer<Type>::findMinMax(channel, startSample + numHistorySamples, numSamples);
		}

		Type getMagnitude(int channel,
			int startSample,
			int numSamples) const noexcept
		{
			return AudioBuffer<Type>::getMagnitude(channel, startSample + numHistorySamples, numSamples);
		}

		Type getMagnitude(int startSample,
			int numSamples) const noexcept
		{
			return AudioBuffer<Type>::getMagnitude(startSample + numHistorySamples, numSamples);
		}

		Type getRMSLevel(int channel,
			int startSample,
			int numSamples) const noexcept
		{
			const Type rms = AudioBuffer<Type>::getRMSLevel(channel, startSample + numHistorySamples, numSamples);
			return isnan(rms) ? 0 : rms;
		}

		void reverse(int channel, int startSample, int numSamples) const noexcept
		{
			AudioBuffer<Type>::reverse(channel, startSample + numHistorySamples, numSamples);
		}

		void reverse(int startSample, int numSamples) const noexcept
		{
			AudioBuffer<Type>::reverse(startSample + numHistorySamples, numSamples);
		}

		void shiftSamplesToHistory(bool clearPresent = false)
		{
			if (numHistorySamples == 0)
				return;

			const int numChanns = getNumChannels();
			for (int i = 0; i < numChanns; ++i)
			{
				memmove(
					getWritePointer(i, -numHistorySamples),
					getReadPointer(i, numCurrentSamples - numHistorySamples),
					(size_t)(numHistorySamples) * sizeof(Type)
				);

				if (clearPresent)
				{
					clear(i, 0, numCurrentSamples);
				}
			}
		}

		String toString(int channel, int startSample, int numSamples, int numOfDecimals = 3) const
		{
			String out;

			for (int i = 0; i < numSamples; ++i)
			{
				const int sampleIndex = startSample + i;

				if (sampleIndex >= -numHistorySamples && sampleIndex < numCurrentSamples)
				{
					out += String(getSample(channel, sampleIndex), numOfDecimals) + " "; 
				}
				else
				{
					out += " - ";
				}
			}

			return out;
		}

		bool hasBrokenData(bool checkForMagnitude = true) const
		{
			if (getNumChannels() == 0 || getNumSamples() == 0) return false;

			for (int chann = 0; chann < getNumChannels(); ++chann)
			{
				const Type* channData = getReadPointer(chann, -numHistorySamples);

				for (int i = 0; i < numHistorySamples + numCurrentSamples; ++i)
				{
					if (
						isnan(channData[i]) || 
						(checkForMagnitude && isinf(channData[i])) ||
						(checkForMagnitude && abs(channData[i]) > (Type)100000)
						)
					{
						return true;
					}
				}
			}

			return false;
		}

		void checkForBrokenData(bool checkForMagnitude = true) const
		{
			ignoreUnused(checkForMagnitude);
			jassert(!hasBrokenData(checkForMagnitude));
		}

		bool containsSampleOver(Type value) const
		{
			return (Type)getMagnitude(0, numCurrentSamples) > value;
		}

	protected:
		int numHistorySamples = 0;
		int numCurrentSamples = 0;

		JUCE_LEAK_DETECTOR(AudioBufferExtended)
	};

	typedef AudioBufferExtended<float> AudioBufferFloat;
	typedef AudioBufferExtended<double> AudioBufferDouble;
}
