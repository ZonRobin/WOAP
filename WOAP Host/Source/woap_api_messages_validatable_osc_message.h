#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_functions.h"

using std::forward;

namespace WOAP
{
	namespace API
	{
		namespace Messages
		{
			class ValidatableOSCMessage
			{
			public:
				ValidatableOSCMessage(const OSCMessage& msg)
					: message(&const_cast<OSCMessage&>(msg), false)
				{
					
				}

				ValidatableOSCMessage()
				{

				}

				virtual ~ValidatableOSCMessage()
				{

				}

				inline virtual bool isValid() const
				{
					if (!isAddressValid() || !areArgumentsValid() || !validate())
					{
						logMessageInvalidity();
						jassertfalse;
						return false;
					}
					else
					{
						return true;
					}
				}

				inline virtual Array<OSCType> getValidArgumentTypes() const
				{
					return Array<OSCType>();
				}

				operator const OSCMessage&()
				{
					return *message;
				}

				inline OSCMessage const* getRawMessage() const
				{
					return message;
				}

				inline virtual bool areArgumentsValid() const
				{
					Array<OSCType> argumentTypes = getValidArgumentTypes();

					if (!hasNumArguments(argumentTypes.size()))
					{
						return false;
					}

					for (OSCType* type = argumentTypes.begin(); type != argumentTypes.end(); ++type)
					{
						if ((message->begin() + (type - argumentTypes.begin()))->getType() != *type)
						{
							return false;
						}
					}

					return true;
				}

				inline bool isAddressValid() const
				{
					#ifdef JUCE_DEBUG
						return true;
					#else			
						return getAddress() == getValidAddress();
					#endif		
				}

				inline const String getAddress() const
				{
					return message->getAddressPattern().toString();
				}

				inline virtual String getValidAddress() const = 0;

				inline virtual bool hasNumArguments(int numArguments) const
				{
					return message->size() == numArguments;
				}

				inline virtual int getNumArguments() const
				{
					return message->size();
				}

				inline virtual OSCArgument& getArgument(int offset) const
				{
					/* You're trying to get non-existing argument */
					jassert(getNumArguments() > offset);

					return *(message->begin() + offset);
				}

				inline virtual String getArgumentString(int offset) const
				{
					return getArgument(offset).getString();
				}

				inline virtual Uuid getArgumentUuid(int offset) const
				{
					return uuidFromDashedString(getArgument(offset).getString());
				}

				inline virtual int getArgumentInt(int offset) const
				{
					return (int)getArgument(offset).getInt32();
				}

				inline virtual bool getArgumentBool(int offset) const
				{
					return intToBool((int)getArgument(offset).getInt32());
				}

				inline virtual float getArgumentFloat(int offset) const
				{
					const float value = getArgument(offset).getFloat32();
					return isnan(value) ? 0 : value;
				}

				inline virtual const MemoryBlock& getArgumentBlob(int offset) const
				{
					return getArgument(offset).getBlob();
				}

			protected:
				OptionalScopedPointer<OSCMessage> message;

				void initializeMessage()
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress()));
				}

				template <typename Arg1, typename... Args>
				void initializeWithArgs(Arg1&& arg1, Args&&... args)
				{
					jassert(message == nullptr);
					message.setOwned(new OSCMessage(getValidAddress(), std::forward<Arg1>(arg1), std::forward<Args>(args)...));
				}

				template <typename Arg1, typename... Args>
				void addArguments(Arg1&& arg1, Args&&... args)
				{
					message->addArgument(arg1);
					addArguments(forward<Args>(args)...);
				}

				void addArguments() {}

				inline virtual bool validate() const 
				{ 
					return true; 
				}

				inline void logMessageInvalidity() const
				{
					#ifdef JUCE_DEBUG
						DBG("OSC message is not valid: " + message->getAddressPattern().toString());
						if (!isAddressValid())
						{
							DBG("Address is not valid. Got " + message->getAddressPattern().toString() + ", expected " + getValidAddress());
						}
						if (!areArgumentsValid())
						{
							DBG("Arguments are not valid");
						}
						if (!validate())
						{
							DBG("Custom validation failed");
						}
					#endif
				}
					
			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ValidatableOSCMessage)
			};


		}
	}
}
