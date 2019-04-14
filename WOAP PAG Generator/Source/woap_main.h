#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Main : public JUCEApplication
{
public:
	const static String PAGVersion;

	Main();

	const String getApplicationName() override;
	const String getApplicationVersion() override;
	bool moreThanOneInstanceAllowed() override;

	void initialise(const String& commandLine) override;
	void shutdown() override;

	void systemRequestedQuit() override;
	void anotherInstanceStarted(const String& commandLine) override;

	static ApplicationCommandManager& getApplicationCommandManager();

	static void invokeCommand(CommandID commandId, bool asynchronously = false);
	
	class MainWindow : public DocumentWindow
	{
	public:
		MainWindow(String name);
		~MainWindow();
		void closeButtonPressed() override;

	protected:
		ScopedPointer<LookAndFeel> lookAndFeel = nullptr;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	};

protected:
	static ScopedPointer<ApplicationCommandManager> commandManager;
	ScopedPointer<MainWindow> mainWindow = nullptr;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Main)
};
