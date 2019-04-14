#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ParsingException;

class PluginParameter
{
public:
	enum Type
	{
		Unknown,
		Switch,
		Float
	};

	static const int MaxNumSteps;

	class Listener
	{
	public:
		virtual void parameterChanged(PluginParameter* parameter) = 0;
		virtual void numStepsChanged(int newNumSteps) { ignoreUnused(newNumSteps); }
		virtual void typeChanged(PluginParameter::Type) {}
	};

	PluginParameter(int indexToUse, String refToUse);
	~PluginParameter();

	int getIndex() const { return index; }
	String getRef() const { return ref; }
	Type getType() const { return type; }
	int getNumSteps() const { return registeredValues.size(); }
	float getValue() const { return value; }
	String getValueText() const { return valueText; }
	const SortedSet<float>& getRegisteredValues() { return registeredValues; }

	void setValueAndText(float newValue, String newValueText);

	void addListener(Listener* listener) { listeners.add(listener); }
	void removeListener(Listener* listener) { listeners.remove(listener); }

	XmlElement* createXml() const;
	static PluginParameter* fromXml(int index, const XmlElement& parameterElement);

protected:
	int index;
	String ref;
	Type type = PluginParameter::Type::Unknown;
	float value = 0;
	String valueText;

	SortedSet<float> registeredValues;
	ListenerList<PluginParameter::Listener> listeners;

	void registerValue(float newValue);
	void updateTypeBySteps();
};