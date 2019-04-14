#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap.h"

namespace WOAP
{
	class Parameter
	{
	public:
		class Listener
		{
		public:
			virtual void parameterChanged(Parameter*) = 0;
			virtual void parameterWillBeDeleted(Parameter*) {};
		};

		Parameter(const String& parameterID, const String& name);
		virtual ~Parameter();

		/* Get parameter value (0 - 1) */
		virtual float getValue() const = 0;

		/* Set parameter value */
		virtual void setValue(float newValue) = 0;

		void setValueNotifyingListeners(float newValue);

		/* Get parameter default value */
		virtual float getDefaultValue() const = 0;

		/* Get number of parameter steps */
		virtual int getNumSteps() const = 0;

		/* Get value for text */
		virtual String getText(float, int) const = 0;

		/* Get text from value */
		virtual float getValueForText(const String&) const = 0;

		String getID() const { return parameterID; }

		String getName() const { return name; }

		/* Get default number of steps */
		static int getDefaultNumSteps()
		{
			return 0x7fffffff;
		}

		void addListener(Listener* listener) { listeners.add(listener); }
		void removeListener(Listener* listener) { listeners.remove(listener); }
		ListenerList<Listener>& getListeners() { return listeners; }

	protected:
		String parameterID;
		String name;
		ListenerList<Listener> listeners;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
	};
}
