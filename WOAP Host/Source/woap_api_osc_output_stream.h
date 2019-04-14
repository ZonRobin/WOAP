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
			struct OutputStream
			{
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

				OutputStream() noexcept {}

				/** Returns a pointer to the data that has been written to the stream. */
				const void* getData() const noexcept { return output.getData(); }

				/** Returns the number of bytes of data that have been written to the stream. */
				size_t getDataSize() const noexcept { return output.getDataSize(); }

				bool writeInt32(int32 value)
				{
					return output.writeIntBigEndian(value);
				}

				bool writeUint64(uint64 value)
				{
					return output.writeInt64BigEndian(int64(value));
				}

				bool writeFloat32(float value)
				{
					return output.writeFloatBigEndian(value);
				}

				bool writeString(const String& value)
				{
					if (!output.writeString(value))
						return false;

					const size_t numPaddingZeros = ~value.length() & 3;

					return output.writeRepeatedByte('\0', numPaddingZeros);
				}

				bool writeBlob(const MemoryBlock& blob)
				{
					if (!(output.writeIntBigEndian((int)blob.getSize())
						&& output.write(blob.getData(), blob.getSize())))
						return false;

					const size_t numPaddingZeros = ~(blob.getSize() - 1) & 3;

					return output.writeRepeatedByte(0, numPaddingZeros);
				}

				bool writeTimeTag(OSCTimeTag timeTag)
				{
					return output.writeInt64BigEndian(int64(timeTag.getRawTimeTag()));
				}

				bool writeAddress(const OSCAddress& address)
				{
					return writeString(address.toString());
				}

				bool writeAddressPattern(const OSCAddressPattern& ap)
				{
					return writeString(ap.toString());
				}

				bool writeTypeTagString(const OSCTypeList& typeList)
				{
					output.writeByte(',');

					if (typeList.size() > 0)
						output.write(typeList.begin(), (size_t)typeList.size());

					output.writeByte('\0');

					size_t bytesWritten = (size_t)typeList.size() + 1;
					size_t numPaddingZeros = ~bytesWritten & 0x03;

					return output.writeRepeatedByte('\0', numPaddingZeros);
				}

				bool writeArgument(const OSCArgument& arg)
				{
					switch (arg.getType())
					{
					case Types::int32:       return writeInt32(arg.getInt32());
					case Types::float32:     return writeFloat32(arg.getFloat32());
					case Types::string:      return writeString(arg.getString());
					case Types::blob:        return writeBlob(arg.getBlob());

					default:
						jassertfalse;
						return false;
					}
				}

				bool writeMessage(const OSCMessage& msg)
				{
					if (!writeAddressPattern(msg.getAddressPattern()))
						return false;

					OSCTypeList typeList;

					for (OSCArgument* arg = msg.begin(); arg != msg.end(); ++arg)
						typeList.add(arg->getType());

					if (!writeTypeTagString(typeList))
						return false;

					for (OSCArgument* arg = msg.begin(); arg != msg.end(); ++arg)
						if (!writeArgument(*arg))
							return false;

					return true;
				}

				bool writeBundle(const OSCBundle& bundle)
				{
					if (!writeString("#bundle"))
						return false;

					if (!writeTimeTag(bundle.getTimeTag()))
						return false;

					for (OSCBundle::Element* element = bundle.begin(); element != bundle.end(); ++element)
						if (!writeBundleElement(*element))
							return false;

					return true;
				}

				bool writeBundleElement(const OSCBundle::Element& element)
				{
					const int64 startPos = output.getPosition();

					if (!writeInt32(0))
						return false;

					if (element.isBundle())
					{
						if (!writeBundle(element.getBundle()))
							return false;
					}
					else
					{
						if (!writeMessage(element.getMessage()))
							return false;
					}

					const int64 endPos = output.getPosition();
					const int64 elementSize = endPos - (startPos + 4);

					return output.setPosition(startPos)
						&& writeInt32((int32)elementSize)
						&& output.setPosition(endPos);
				}

			private:
				MemoryOutputStream output;

				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputStream)
			};
		}
	}
}
