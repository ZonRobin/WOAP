#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Singleton
{
public:
	class Deletable
	{
	public:
		Deletable() {}
		virtual ~Deletable() {}
	};

	template<class ClassType>
	class Instance : public Deletable
	{
	public:
		Instance(Instance<ClassType>** selfPtr, ClassType** ptr) : selfPointer(selfPtr), pointer(ptr) 
		{ 
			if (instances == nullptr)
			{
				instances = new OwnedArray<Deletable>();
			}

			instances->add(this); 
		}

		virtual ~Instance() 
		{ 
			instances->removeObject(this, false);
			ScopedPointer<ClassType> bye(*pointer);
			
			pointer = nullptr;
			selfPointer = nullptr;
		}

	protected:
		Instance<ClassType>** selfPointer;
		ClassType** pointer;
	};

	static void deleteInstances();

protected:
	static OwnedArray<Deletable>* instances;
};

#define SINGLETON(ClassType) \
\
	public: \
		virtual ~ClassType(); \
\
		inline static ClassType* getInstance() \
		{ \
			if (_instance == nullptr) \
			{ \
				_instance = new ClassType(); \
				_singletonInstance = new Singleton::Instance<ClassType>(&_singletonInstance, &_instance); \
			} \
\
			return _instance; \
		} \
\
		inline static ClassType* gI() { return getInstance(); } \
		inline static void deleteInstance() { ScopedPointer<Singleton::Instance<ClassType>> si(_singletonInstance); } \
\
	private: \
		ClassType(); \
		 \
		static ClassType* _instance; \
		static Singleton::Instance<ClassType>* _singletonInstance; \
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassType) \
\

#define OPTIONAL_SINGLETON(ClassType) \
\
	public: \
		virtual ~ClassType(); \
\
		inline static ClassType* getInstance() { return _instance; } \
\
		inline static ClassType* createInstanceIfNeeded() \
		{ \
			jassert(_instance == nullptr); \
\
			if (_instance == nullptr) \
				_instance = new ClassType(); \
\
			return _instance; \
		} \
\
		inline static ClassType* gI() { return getInstance(); } \
\
	private: \
		ClassType(); \
		 \
		static ClassType* _instance; \
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassType) \
\

#define SINGLETON_INLINE(ClassType) \
\
	public: \
		inline static ClassType* getInstance() \
		{ \
			if (_instance == nullptr) \
			{ \
				_instance = new ClassType(); \
				_singletonInstance = new Singleton::Instance<ClassType>(&_singletonInstance, &_instance); \
			} \
\
			return _instance; \
		} \
\
		inline static void createInstance() \
		{ \
			if (_instance == nullptr) \
			{ \
				_instance = new ClassType(); \
				_singletonInstance = new Singleton::Instance<ClassType>(&_singletonInstance, &_instance); \
			} \
		} \
\
		inline static ClassType* gI() { return getInstance(); } \
		inline static void deleteInstance() { ScopedPointer<Singleton::Instance<ClassType>> si(_singletonInstance); } \
\
	private: \
		static ClassType* _instance; \
		static Singleton::Instance<ClassType>* _singletonInstance; \
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ClassType) \
\



/* --------------------------------------------------------------- */

#define SINGLETON_IMPLEMENTATION(ClassType) \
	ClassType* ClassType::_instance = nullptr; \
	Singleton::Instance<ClassType>* ClassType::_singletonInstance  = nullptr; \
\


#define OPTIONAL_SINGLETON_IMPLEMENTATION(ClassType) \
	ClassType* ClassType::_instance = nullptr; \
\

