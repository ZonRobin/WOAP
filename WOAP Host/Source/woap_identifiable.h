#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_functions.h"

namespace WOAP
{
	class Identifiable
	{
	public:
		Identifiable() {}
		Identifiable(const Uuid& idToUse) : id(idToUse) {}
		Identifiable(const String& idToUse) : id(uuidFromDashedString(idToUse)) {}
		virtual ~Identifiable() {}

		Uuid getId() const noexcept { return id; }

	protected:
		void setId(const Uuid& newId) { id = newId; }
		void setId(const String& newId) { id = uuidFromDashedString(newId); }

	private:
		Uuid id;
	};
}
