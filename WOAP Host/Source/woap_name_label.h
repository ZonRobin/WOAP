#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	namespace GUI
	{
		class NameLabel : public Label
		{
		public:
			NameLabel();
			~NameLabel() {}

			void setFontSize(float newSize);
			void setBold(bool bold);
			void setMaxInputLength(int maxLength);
			void setBackgroundColour(Colour newColour);
			void setBackgroundColour(Colour newColour, bool useDarkText);
			void setBackgroundMargin(int newMarginX, int newMarginY) { backgroundMarginX = newMarginX; backgroundMarginY = newMarginY; repaint(); }
			
			int getBackgroundMarginX() const { return backgroundMarginX; }
			int getBackgroundMarginY() const { return backgroundMarginY; }

			TextEditor* createEditorComponent() override;

		protected:
			int maxInputLength;
			int backgroundMarginX = 0;
			int backgroundMarginY = 0;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NameLabel)
		};

		class MixerNodeNameLabel : public NameLabel, public MixerNode::Listener
		{
		public:
			MixerNodeNameLabel(bool allowEdit, MixerNode* node = nullptr);
			virtual ~MixerNodeNameLabel();

			void setMixerNode(MixerNode* mnode);

			virtual void nameChanged(MixerNode*) override;
			virtual void colourChanged(MixerNode*) override;
			virtual void aboutToDelete(MixerNode*) override;

		protected:
			MixerNode* mixerNode = nullptr;

			virtual void textWasChanged() override;
			void updateColours();

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerNodeNameLabel)
		};
	}
}
