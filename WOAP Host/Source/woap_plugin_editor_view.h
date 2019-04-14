#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_window.h"
#include "woap_audio_processor_player_double.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	namespace GUI
	{
		class ButtonView;
		class NameLabel;

		class PluginEditorView : public Window, public AudioProcessorPlayerDouble::Listener
		{
		public:
			class EditorWrapper : public Component, public AudioProcessorPlayerDouble::Listener, public MixerNode::Listener, public Label::Listener, public Button::Listener
			{
			public:
				EditorWrapper(AudioProcessorPlayerDouble* plugin);
				~EditorWrapper();

				void resized() override;
				void paint(Graphics& g) override;
				virtual void childBoundsChanged(Component* child) override;

				bool canDragEditor(const MouseEvent&);

				virtual void nameChanged(AudioProcessorPlayerDouble* plugin) override;
				virtual void aboutToDelete(AudioProcessorPlayerDouble*) override {}

				virtual void nameChanged(MixerNode* node) override;
				virtual void colourChanged(MixerNode*) override {}
				virtual void aboutToDelete(MixerNode*) override {}

				virtual void labelTextChanged(Label*) override {}
				virtual void editorShown(Label*, TextEditor&) override;
				virtual void editorHidden(Label*, TextEditor&) override;

				virtual void buttonClicked(Button*) override;

			protected:
				int menuHeight = 0;
				int menuLineHeight = 0;
				int minWidth = 0;
				int menuLineBreakHeight = 0;
				AudioProcessorPlayerDouble* plugin;
				ScopedPointer<AudioProcessorEditor> pluginEditor;

				ScopedPointer<NameLabel> nameLabel;
				ScopedPointer<Button> closeButton;
				ScopedPointer<ButtonView> bypassButton;
				ScopedPointer<ButtonView> activeStateParameter;
				
			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorWrapper)
			};

			PluginEditorView(CloseWindowListener* owner, AudioProcessorPlayerDouble* plugin);
			virtual ~PluginEditorView();

			AudioProcessorPlayerDouble const* getPlugin() const { return plugin; }

			void resized() override;
			void mouseDown(const MouseEvent&) override;
			void mouseUp(const MouseEvent&) override;

			virtual void nameChanged(AudioProcessorPlayerDouble*) override {}
			virtual void aboutToDelete(AudioProcessorPlayerDouble* plugin) override;

		protected:
			AudioProcessorPlayerDouble* plugin;
			EditorWrapper* editorWrapper;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorView)
		};
	}
}
