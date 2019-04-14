#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"

using std::initializer_list;

namespace WOAP
{
	class UserPermission;

	class Bootstrapper
	{
	public:
		class Initializer
		{
		public:
			Initializer(void(*initializerCallback) (Bootstrapper&));
			virtual ~Initializer();

		protected:
			void(*initCallback) (Bootstrapper&);

			void initialize(Bootstrapper& bootstrapper);

		private:
			friend class Bootstrapper;
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Initializer)
		};

		virtual ~Bootstrapper();
		static Bootstrapper* getInstance();
		static void deleteInstance();
		
		void callInitializers();
		const Array<Bootstrapper::Initializer*>& getInitializers() const;

		void reportPermissions(const initializer_list<UserPermission*>& permissions);
		const OwnedArray<UserPermission>& getReportedPermissions() const;

	protected:
		void addInitializer(Bootstrapper::Initializer* initializer);
		void removeInitializer(Bootstrapper::Initializer* initializer);

		ListenerList<Bootstrapper::Initializer> initializers;
		bool initializersCalled = false;
		OwnedArray<UserPermission> reportedPermissions;

	private:
		Bootstrapper();
		static Bootstrapper* instance;
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Bootstrapper)
	};	
}
