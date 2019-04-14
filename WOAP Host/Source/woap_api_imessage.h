#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace WOAP
{
	namespace API
	{
		struct AddressWithTypeId
		{
			AddressWithTypeId(const String& addr, int id = 0)
				: address(addr), typeId(id)
			{

			}

			String address;
			int typeId;
		};

		class IMessage
		{
		public:
			virtual ~IMessage() {}

			virtual String toString() const = 0;
		};
	}
}
