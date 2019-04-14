#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_singleton.h"

namespace WOAP
{
	class AppCore;

	namespace GUI
	{
		class AudioCoreView;
		class SettingsView;
		class ThrashButton;
		class IconButton;
		class LoadingSpinner;

		class MainView : 
			public Component,
			public Button::Listener,
			public DragAndDropContainer
		{
		public:
			void resized() override;
			void paint(Graphics& g) override;
			virtual void buttonClicked(Button* button) override;
			virtual void mouseUp(const MouseEvent& event) override;

			AudioCoreView* getAudioCoreView() const { return audioCoreView; }

			void showAudioCore();
			void showSettings();
			void setAppLoaded(bool loaded);

			virtual void dragOperationStarted(const DragAndDropTarget::SourceDetails&) override;
			virtual void dragOperationEnded(const DragAndDropTarget::SourceDetails&) override;

		protected:
			ScopedPointer<AudioCoreView> audioCoreView;
			ScopedPointer<SettingsView> settingsView;
			ScopedPointer<LoadingSpinner> loadingSpinner;

			ScopedPointer<IconButton> addTrackButton;
			ScopedPointer<IconButton> settingsButton;
			ScopedPointer<IconButton> mixerButton;
			ScopedPointer<ThrashButton> thrashButton;

			bool appLoaded = false;

			static const char* backgroundLogo;
			static const int backgroundLogoSize;
			Image cachedBackgroundLogo;

		private:
			OPTIONAL_SINGLETON(MainView)
		};

		class MainWindow : public DocumentWindow, public ApplicationCommandTarget
		{
		public:
			MainWindow(MainView* mainView, const String& name, bool enableKioskModeToggle, bool startInKioskMode);
			virtual ~MainWindow();

			void closeButtonPressed() override;

			void enterKioskMode();
			void exitKioskMode();

			ApplicationCommandTarget* getNextCommandTarget() override { return nullptr; }
			void getCommandInfo(CommandID, ApplicationCommandInfo&) override;
			void getAllCommands(Array<CommandID>&) override;
			bool perform(const InvocationInfo&) override;

		protected:
			bool isInKioskMode = false;
			bool kioskModeToggleEnabled = false;
			const int toggleKioskModeCommandId = 50000;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
		};
	}
}
