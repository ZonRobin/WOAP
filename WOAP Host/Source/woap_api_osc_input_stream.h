#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace API
	{
		namespace OSC
		{
			/*
				This class is part of JUCE library
			*/
			/** Allows a block of data to be accessed as a stream of OSC data.

			The memory is shared and will be neither copied nor owned by the InputStream.

			This class is implementing the Open Sound Control 1.0 Specification for
			interpreting the data.

			Note: some older implementations of OSC may omit the OSC Type Tag string
			in OSC messages. This class will treat such OSC messages as format errors.
			*/
			class InputStream
			{
			public:
				class Types
				{
				public:
					static const OSCType int32 = 'i';
					static const OSCType float32 = 'f';
					static const OSCType string = 's';
					static const OSCType blob = 'b';

					static bool isSupportedType(OSCType type) noexcept
					{
						return type == Types::int32
							|| type == Types::float32
							|| type == Types::string
							|| type == Types::blob;
					}
				};

				/** Creates an InputStream.

				@param sourceData               the block of data to use as the stream's source
				@param sourceDataSize           the number of bytes in the source data block
				*/
				InputStream(const void* sourceData, size_t sourceDataSize)
					: input(sourceData, sourceDataSize, false)
				{}

				/** Returns a pointer to the source data block from which this stream is reading. */
				const void* getData() const noexcept { return input.getData(); }

				/** Returns the number of bytes of source data in the block from which this stream is reading. */
				size_t getDataSize() const noexcept { return input.getDataSize(); }

				/** Returns the current position of the stream. */
				uint64 getPosition() { return uint64(input.getPosition()); }

				/** Attempts to set the current position of the stream. Returns true if this was successful. */
				bool setPosition(int64 pos) { return input.setPosition(pos); }

				/** Returns the total amount of data in bytes accessible by this stream. */
				int64 getTotalLength() { return input.getTotalLength(); }

				/** Returns true if the stream has no more data to read. */
				bool isExhausted() { return input.isExhausted(); }

				int32 readInt32()
				{
					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading int32");

					return input.readIntBigEndian();
				}

				uint64 readUint64()
				{
					if (input.getNumBytesRemaining() < 8)
						throw OSCFormatError("OSC input stream exhausted while reading uint64");

					return (uint64)input.readInt64BigEndian();
				}

				float readFloat32()
				{
					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading float");

					return input.readFloatBigEndian();
				}

				String readString()
				{
					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading string");

					const size_t posBegin = (size_t)getPosition();
					String s(input.readString());
					const size_t posEnd = (size_t)getPosition();

					if (static_cast<const char*> (getData())[posEnd - 1] != '\0')
						throw OSCFormatError("OSC input stream exhausted before finding null terminator of string");

					size_t bytesRead = posEnd - posBegin;
					readPaddingZeros(bytesRead);

					return s;
				}

				MemoryBlock readBlob()
				{
					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading blob");

					const size_t blobDataSize = (size_t)input.readIntBigEndian();

					if ((size_t)input.getNumBytesRemaining() < (blobDataSize + 3) % 4)
						throw OSCFormatError("OSC input stream exhausted before reaching end of blob");

					MemoryBlock blob;

					const size_t bytesRead = input.readIntoMemoryBlock(blob, (ssize_t)blobDataSize);
					readPaddingZeros(bytesRead);

					return blob;
				}

				OSCTimeTag readTimeTag()
				{
					if (input.getNumBytesRemaining() < 8)
						throw OSCFormatError("OSC input stream exhausted while reading time tag");

					return OSCTimeTag(uint64(input.readInt64BigEndian()));
				}

				OSCAddress readAddress()
				{
					return OSCAddress(readString());
				}

				OSCAddressPattern readAddressPattern()
				{
					return OSCAddressPattern(readString());
				}

				OSCTypeList readTypeTagString()
				{
					OSCTypeList typeList;

					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading type tag string");

					if (input.readByte() != ',')
						throw OSCFormatError("OSC input stream format error: expected type tag string");

					for (;;)
					{
						if (isExhausted())
							throw OSCFormatError("OSC input stream exhausted while reading type tag string");

						const OSCType type = input.readByte();

						if (type == 0)
							break;

						if (!OSCTypes::isSupportedType(type))
							throw OSCFormatError("OSC input stream format error: encountered unsupported type tag");

						typeList.add(type);
					}

					size_t bytesRead = (size_t)typeList.size() + 2;
					readPaddingZeros(bytesRead);

					return typeList;
				}

				OSCArgument readArgument(OSCType type)
				{
					switch (type)
					{
					case Types::int32:       return OSCArgument(readInt32());
					case Types::float32:     return OSCArgument(readFloat32());
					case Types::string:      return OSCArgument(readString());
					case Types::blob:        return OSCArgument(readBlob());

					default:
						jassertfalse;
						throw OSCInternalError("OSC input stream: internal error while reading message argument");
					}
				}

				OSCMessage readMessage()
				{
					OSCAddressPattern ap = readAddressPattern();
					OSCTypeList types = readTypeTagString();

					OSCMessage msg(ap);

					for (OSCType* type = types.begin(); type != types.end(); ++type)
						msg.addArgument(readArgument(*type));

					return msg;
				}

				OSCBundle readBundle(size_t maxBytesToRead = std::numeric_limits<size_t>::max())
				{
					/* maxBytesToRead is only passed in here in case this bundle is a nested
					 bundle, so we know when to consider the next element *not* part of this
					 bundle anymore (but part of the outer bundle) and return. */

					if (input.getNumBytesRemaining() < 16)
						throw OSCFormatError("OSC input stream exhausted while reading bundle");

					if (readString() != "#bundle")
						throw OSCFormatError("OSC input stream format error: bundle does not start with string '#bundle'");

					OSCBundle bundle(readTimeTag());

					size_t bytesRead = 16;
					size_t pos = (size_t)getPosition();

					while (!isExhausted() && bytesRead < maxBytesToRead)
					{
						bundle.addElement(readElement());

						const size_t newPos = (size_t)getPosition();
						bytesRead += newPos - pos;
						pos = newPos;
					}

					return bundle;
				}

				OSCBundle::Element readElement()
				{
					if (input.getNumBytesRemaining() < 4)
						throw OSCFormatError("OSC input stream exhausted while reading bundle element size");

					const size_t elementSize = (size_t)readInt32();

					if (elementSize < 4)
						throw OSCFormatError("OSC input stream format error: invalid bundle element size");

					return readElementWithKnownSize(elementSize);
				}

				OSCBundle::Element readElementWithKnownSize(size_t elementSize)
				{
					if ((uint64)input.getNumBytesRemaining() < elementSize)
						throw OSCFormatError("OSC input stream exhausted while reading bundle element content");

					const char firstContentChar = static_cast<const char*> (getData())[getPosition()];

					if (firstContentChar == '/')  return OSCBundle::Element(readMessageWithCheckedSize(elementSize));
					if (firstContentChar == '#')  return OSCBundle::Element(readBundleWithCheckedSize(elementSize));

					throw OSCFormatError("OSC input stream: invalid bundle element content");
				}

			private:
				MemoryInputStream input;

				void readPaddingZeros(size_t bytesRead)
				{
					size_t numZeros = ~(bytesRead - 1) & 0x03;

					while (numZeros > 0)
					{
						if (isExhausted() || input.readByte() != 0)
							throw OSCFormatError("OSC input stream format error: missing padding zeros");

						--numZeros;
					}
				}

				OSCBundle readBundleWithCheckedSize(size_t size)
				{
					const size_t begin = (size_t)getPosition();
					const size_t maxBytesToRead = size - 4;

					OSCBundle bundle(readBundle(maxBytesToRead));

					if (getPosition() - begin != size)
						throw OSCFormatError("OSC input stream format error: wrong element content size encountered while reading");

					return bundle;
				}

				OSCMessage readMessageWithCheckedSize(size_t size)
				{
					const size_t begin = (size_t)getPosition();
					OSCMessage message(readMessage());

					if (getPosition() - begin != size)
						throw OSCFormatError("OSC input stream format error: wrong element content size encountered while reading");

					return message;
				}
			};
		}
	}
}
