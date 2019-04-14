#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_functions.h"
#include "woap_identifiable.h"

namespace WOAP
{
	class Stateful
	{
	public:
		virtual ValueTree getState() const = 0;
		virtual void setState(const ValueTree& state) = 0;
	};

	namespace StateHelper
	{
		inline Uuid getIdProperty(const ValueTree& state, const String& propName)
		{
			const var prop = state.getProperty(propName);
			if (!prop.isString())
				return Uuid();

			return uuidFromDashedString(prop.toString());
		}

		inline Uuid getIdProperty(const ValueTree& state)
		{
			return getIdProperty(state, "id");
		}

		inline void setIdProperty(ValueTree& state, const String& propName, const Identifiable* identifiable)
		{
			state.setProperty(propName, identifiable->getId().toDashedString(), nullptr);
		}

		inline void setIdProperty(ValueTree& state, const Identifiable* identifiable)
		{
			setIdProperty(state, "id", identifiable);
		}

		inline String getStringProperty(const ValueTree& state, const String& propName, const String& defaultValue = String())
		{
			const var prop = state.getProperty(propName);
			if (!prop.isString())
				return defaultValue;

			return prop.toString();
		}

		inline int getIntProperty(const ValueTree& state, const String& propName, int defaultValue = 0)
		{
			const var prop = state.getProperty(propName);

			if (prop.isString()) 
			{
				const String propVal = prop.toString();
				return CharacterFunctions::getIntValue<int>(propVal.toUTF8());
			}

			if (!prop.isInt())
				return defaultValue;

			return (int)prop;
		}

		inline int64 getInt64Property(const ValueTree& state, const String& propName, int64 defaultValue = 0)
		{
			const var prop = state.getProperty(propName);

			if (prop.isString())
			{
				const String propVal = prop.toString();
				return CharacterFunctions::getIntValue<int64>(propVal.toUTF8());
			}

			if (!prop.isInt64() && !prop.isInt())
				return defaultValue;

			return (int64)prop;
		}

		inline double getDoubleProperty(const ValueTree& state, const String& propName, double defaultValue = 0)
		{
			const var prop = state.getProperty(propName);

			if (prop.isString()) 
			{
				const String propVal = prop.toString();
				const double value = CharacterFunctions::getDoubleValue(propVal.toUTF8());
				return isnan(value) ? 0 : value;
			}
			
			if (!prop.isDouble())
				return defaultValue;

			return (double)prop;
		}

		inline float getFloatProperty(const ValueTree& state, const String& propName, float defaultValue = 0)
		{
			return (float)getDoubleProperty(state, propName, (float)defaultValue);
		}

		inline bool getBoolProperty(const ValueTree& state, const String& propName, bool defaultValue = false)
		{
			const var prop = state.getProperty(propName);

			if (prop.isString()) 
			{
				const String propVal = prop.toString();
				return propVal.isNotEmpty() && propVal != "0";
			}
				
			if (!prop.isBool())
				return defaultValue;

			return (bool)prop;
		}
	}
	
}