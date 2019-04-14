#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_window.h"

class ViewToolbar;
class ViewEditor;
class PluginParameter;
class LoadingScreenComponent;
class HelpBox;

class MainComponent : 
	public AudioAppComponent, 
	public MenuBarModel, 
	public ApplicationCommandTarget, 
	public Window::CloseWindowListener, 
	public AudioProcessorListener, 
	public DragAndDropContainer,
	public FileDragAndDropTarget
{
public:
	enum Command
	{
		OpenPluginFile = 5000,
		ExportAs,
		OpenPluginEditor,
		OpenConfigFile,
		UnloadPlugin,
		ClosePluginEditor,
		CreatePluginSnapshot,
		StartTutorial,
		StopTutorial
	};

	enum class PluginLoadingResult
	{
		Ok,
		NoPluginRecognized,
		ManyPluginsRecognized,
		PluginCreationFailed
	};

	enum class ConfigLoadingResult
	{
		Ok,
		WrongConfigVersion,
		NoPluginsFound,
		MultiplePluginsFounds,
		ConfigPluginDifferent,
		ConfigError
	};

	class Listener
	{
	public:
		virtual void pluginWillBeDeleted() = 0;
		virtual void pluginLoaded(AudioProcessor*, PluginDescription*) = 0;
		virtual void pluginEditorWillBeDeleted() = 0;
		virtual void pluginEditorCreated(AudioProcessorEditor*) = 0;
		virtual void pluginParameterTouched(PluginParameter*) = 0;
	};

	static const int MIN_HEIGHT;
	static const int MIN_WIDTH;

	MainComponent();
	~MainComponent();

	void addListener(MainComponent::Listener* listener);
	void removeListener(MainComponent::Listener* listener);

	int getViewToolbarWidth() const;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;

	void releaseResources() override;

	void paint(Graphics& g) override;

	void resized() override;
	void resizeToFitSnapshot(Image& snapshot);

	virtual StringArray getMenuBarNames() override;

	virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);

	virtual void menuItemSelected(int menuItemID, int topLevelMenuIndex);

	virtual ApplicationCommandTarget* getNextCommandTarget() override;

	virtual void getAllCommands(Array<CommandID>& commands) override;

	virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;

	virtual bool perform(const InvocationInfo& info) override;

	String showPluginFileChooser() const;
	void showExportAsFileChooser();

	void showConfigFileChooser();
	ConfigLoadingResult openConfigByPath(const String& path);

	virtual bool isInterestedInFileDrag(const StringArray& files) override;
	virtual void filesDropped(const StringArray& files, int x, int y) override;

	MainComponent::PluginLoadingResult openPluginByPath(const String& path, bool openEditor = true, bool createSnapshot = true);
	void unloadPlugin();
	bool isPluginLoaded() const;
	AudioProcessor* getPlugin() const;

	const OwnedArray<PluginParameter>& getPluginParameters() const { return pluginParameters; }
	PluginParameter* getPluginParameter(int index) const { return pluginParameters.getUnchecked(index); }
	PluginParameter* getPluginParameter(const String& ref);

	void pluginChanged();

	AudioProcessorEditor* openPluginEditor();
	void closePluginEditor();

	AudioProcessorEditor* getPluginEditor();
	bool isPluginEditorOpened() const;

	virtual void closeWindowTriggered(Window* closedWindow) override;

	void createPluginSnapshot();
	void setPluginSnapshot(Image& snapshot);

	void setParameterValue(int index, float newValue);
	void parameterTouched(int parameterIndex);

	void showLoadingScreen();
	void hideLoadingScreen();

	virtual void audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue) override;
	virtual void audioProcessorChanged(AudioProcessor* processor) override;

	ViewEditor* getViewEditor() { return viewEditor; }	
	bool isHelpBoxOpened() { return helpBox != nullptr; }

	void exportAsXmlTo(const String& filePathToExportTo) const;
	void loadFromXml(const XmlElement* pluginElement);

protected:
	ListenerList<MainComponent::Listener> listeners;

	ScopedPointer<Window> pluginEditor;
	ScopedPointer<AudioProcessor> plugin;
	ScopedPointer<PluginDescription> pluginDescription;
	OwnedArray<PluginParameter> pluginParameters;
	PluginParameter* activeParameter = nullptr;

	ScopedPointer<LoadingScreenComponent> loadingScreen;
	ScopedPointer<MenuBarComponent> menuBar;
	ScopedPointer<ViewEditor> viewEditor;
	ScopedPointer<ViewToolbar> viewToolbar;
	ScopedPointer<HelpBox> helpBox;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

namespace juce
{
	Image createSnapshotOfNativeWindow(void*);
}