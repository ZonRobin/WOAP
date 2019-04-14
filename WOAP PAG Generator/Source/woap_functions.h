#pragma once

#include "../JuceLibraryCode/JuceHeader.h"


MemoryOutputStream* memoryBlockToBase64(const MemoryBlock& mb, bool addNewLines = true);
MemoryOutputStream* fileToBase64(const File& file, bool addNewLines = true);
MemoryOutputStream* imageToBase64(const Image& image, bool addNewLines = true);

Image base64ToImage(const String& data);