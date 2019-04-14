#include "woap_functions.h"

MemoryOutputStream* memoryBlockToBase64(const MemoryBlock& mb, bool addNewLines)
{
	const short rowSize = 120;

	ScopedPointer<MemoryOutputStream> encoded = new MemoryOutputStream(mb.getSize());
	Base64::convertToBase64(*encoded, mb.getData(), mb.getSize());

	if (!addNewLines || encoded->getDataSize() < rowSize)
	{
		return encoded.release();
	}

	const char* const encodedData = (const char*)encoded->getData();
	const size_t encodedDataSize = encoded->getDataSize();
	MemoryOutputStream* outputStream = new MemoryOutputStream(encodedDataSize + encodedDataSize / 20);
	MemoryOutputStream& out = *outputStream;

	out << "\r\n";

	size_t readIndex;
	for (readIndex = 0; readIndex < encodedDataSize - 1; ++readIndex)
	{
		if ((readIndex % rowSize) != (rowSize - 1))
			out << encodedData[readIndex];
		else
			out << encodedData[readIndex] << "\r\n";
	}

	out << encodedData[readIndex] << "\r\n";

	return outputStream;
}

MemoryOutputStream* fileToBase64(const File& file, bool addNewLines)
{
	MemoryBlock mb;
	file.loadFileAsData(mb);

	return memoryBlockToBase64(mb, addNewLines);
}

MemoryOutputStream* imageToBase64(const Image& image, bool addNewLines)
{
	MemoryOutputStream dataStream;

	PNGImageFormat format;
	format.writeImageToStream(image, dataStream);

	return memoryBlockToBase64(dataStream.getMemoryBlock(), addNewLines);
}

Image base64ToImage(const String& input)
{
	const String data = input.removeCharacters("\r\n ");
	MemoryOutputStream outputStream(data.length());

	Base64::convertFromBase64(outputStream, data);

	return ImageCache::getFromMemory(outputStream.getData(), outputStream.getDataSize());
}
