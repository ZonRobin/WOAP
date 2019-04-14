#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_core.h"
#include "woap_flex_list.h"
#include "woap_mixer_node.h"

namespace WOAP
{
	class Track;
	class MixerSourceNode;
	class MixerReceiverNode;

	namespace GUI
	{
		class MixerIONodeSelectorNode;

		class MixerInputNodeSelector : public FlexList<MixerIONodeSelectorNode>, public AudioCore::Listener
		{
		public:
			MixerInputNodeSelector(int width, MixerReceiverNode* receiver = nullptr);
			virtual ~MixerInputNodeSelector();

			void setReceiverNode(MixerReceiverNode* receiver);

			void paint(Graphics& g) override;
			void updateList();

			virtual void connectionsChanged() override;
			virtual void aboutToDelete(AudioCore*) override {}
			
		protected:
			MixerReceiverNode* receiverNode = nullptr;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerInputNodeSelector)
		};

		class MixerOutputNodeSelector : public FlexList<MixerIONodeSelectorNode>, public AudioCore::Listener
		{
		public:
			MixerOutputNodeSelector(int width, MixerSourceNode* source = nullptr);
			virtual ~MixerOutputNodeSelector();

			void setSourceNode(MixerSourceNode* source);

			void paint(Graphics& g) override;
			void updateList();

			virtual void connectionsChanged() override;
			virtual void aboutToDelete(AudioCore*) override {}

		protected:
			MixerSourceNode* sourceNode = nullptr;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerOutputNodeSelector)
		};

		class MixerIONodeSelectorNode : public Component, public MixerNode::Listener
		{
		public:
			MixerIONodeSelectorNode(bool sourceNode, MixerSourceNode* source, MixerReceiverNode* receiver);
			virtual ~MixerIONodeSelectorNode();

			MixerSourceNode* getSourceNode() const { return source; }
			MixerReceiverNode* getReceiverNode() const { return receiver; }

			void paint(Graphics& g) override;
			virtual void mouseUp(const MouseEvent& event) override;

			virtual void nameChanged(MixerNode* node) override;
			virtual void colourChanged(MixerNode*) override {}
			virtual void aboutToDelete(MixerNode*) override {}

		protected:
			MixerSourceNode* source;
			MixerReceiverNode* receiver;

			String nameToShow;
			bool selected;
			bool isSourceNode;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerIONodeSelectorNode)
		};
	}
}
