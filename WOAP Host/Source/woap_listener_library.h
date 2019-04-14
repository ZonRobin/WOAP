#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>

using std::map;

namespace WOAP
{
	template <class ListenerClass, 
		class ArrayType = Array<ListenerClass*>
	>
	class ListenerLibrary
	{
	public:
		struct Item
		{
		public:
			Item()
			{

			}

			map<String, ScopedPointer<Item>> folders;
			ArrayType listeners;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Item)
		};

		ListenerLibrary(StringRef pathDelimiterChar = "/")
			: pathDelimiter(pathDelimiterChar)
		{
			root = new Item();
		}

		virtual ~ListenerLibrary()
		{

		}

		ArrayType* getListenersOrNull(const String& path) const
		{
			StringArray parts = StringArray::fromTokens(path, pathDelimiter, "");
			Item* folder = root;

			for (String& pathPart : parts)
			{
				auto foundFolder = folder->folders.find(pathPart);

				if (foundFolder == folder->folders.end())
				{
					return nullptr;
				}

				folder = foundFolder->second;
			}

			return &(folder->listeners);
		}

		ArrayType& getListeners(const String& path)
		{
			StringArray parts = StringArray::fromTokens(path, pathDelimiter, "");
			Item* folder = root;

			for (String& pathPart : parts)
			{
				auto foundFolder = folder->folders.find(pathPart);

				if (foundFolder == folder->folders.end())
				{
					Item* newItem = new Item();
					folder->folders[pathPart] = newItem;
					folder = newItem;
				}
				else
				{
					folder = foundFolder->second;
				}
			}

			return folder->listeners;
		}
		
		void addListener(const String& path, ListenerClass* listener)
		{
			/* You should not add nullptr */
			jassert(listener != nullptr);

			getListeners(path).add(listener);
		}

		void addListenerUnchecked(const String& path, ListenerClass* listener)
		{
			/* You should not add nullptr */
			jassert(listener != nullptr);

			StringArray parts = StringArray::fromTokens(path, pathDelimiter, "");
			Item* ownFolder = root;

			for (String& pathPart : parts)
			{
				/* Folder was not found */
				jassert(ownFolder->folders.count(pathPart) != 0);

				ownFolder = ownFolder->folders[pathPart];
			}

			ownFolder->listeners.add(listener);
		}

		void removeListener(const String& path, ListenerClass* listener)
		{
			/* You should not remove nullptr */
			jassert(listener != nullptr);

			ArrayType* listeners = getListenersOrNull(path);

			if (listeners != nullptr)
			{
				removeItemFromArray<ListenerClass, ArrayType>(*listeners, listener);
			}
		}

	protected:
		ScopedPointer<Item> root;
		StringRef pathDelimiter;

		template <class ListenerClass, class ArrayType>
		inline void removeItemFromArray(ArrayType& arr, ListenerClass* item)
		{
			arr.removeFirstMatchingValue(item);
		}

		template <>
		inline void removeItemFromArray<ListenerClass, OwnedArray<ListenerClass>>(OwnedArray<ListenerClass>& arr, ListenerClass* item)
		{
			arr.removeObject(item);
		}

		template <class ListenerClass, class ArrayType>
		inline void removeFromArray(ArrayType& arr, int index)
		{
			arr.remove(index);
		}

		template <class ListenerClass>
		inline void removeFromArray(OwnedArray<ListenerClass>& arr, int index, bool deleteObject = true)
		{
			arr.remove(index, deleteObject);
		}
	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ListenerLibrary)
	};
}
