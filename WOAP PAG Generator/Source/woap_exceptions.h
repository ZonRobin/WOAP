#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ParsingException : public std::exception
{
public:
	explicit ParsingException(String msg = String()) : message(msg) {}
	ParsingException(const ParsingException& other) : message(other.message) {}
	virtual ~ParsingException() noexcept {}
	String getMessage() const { return message; }
	virtual const char* what() const throw () { return message.getCharPointer(); }

protected:
	String message;

private:
	JUCE_LEAK_DETECTOR(ParsingException)
};

class UnmatchingConfigException : public std::exception
{
public:
	explicit UnmatchingConfigException(String msg = String()) : message(msg) {}
	UnmatchingConfigException(const UnmatchingConfigException& other) : message(other.message) {}
	virtual ~UnmatchingConfigException() noexcept {}
	String getMessage() const { return message; }
	virtual const char* what() const throw () { return message.getCharPointer(); }

protected:
	String message;

private:
	JUCE_LEAK_DETECTOR(UnmatchingConfigException)
};