#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_flex_list.h"

namespace WOAP
{
	namespace GUI
	{
		class ColourPalette : public Component
		{
		public:
			class Listener
			{
			public:
				virtual void paletteColourSelected(Colour colour) = 0;
			};

			class Node : public Component
			{
			public:
				Node(Colour c, ColourPalette* p) : colour(c), palette(p) { setMouseCursor(MouseCursor::PointingHandCursor); }
				~Node() {}

				void paint(Graphics& g) override { g.fillAll(colour); }
				virtual void mouseUp(const MouseEvent&) override { palette->nodeClicked(this); }
				Colour getColour() const { return colour; }

			protected:
				Colour colour;
				ColourPalette* palette;

			private:
				JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Node)
			};

			ColourPalette();
			virtual ~ColourPalette();

			virtual void resized() override;

			void nodeClicked(Node* node);
			void addListener(Listener* newListener) { listeners.add(newListener); }
			void removeListener(Listener* listenerToRemove) { listeners.remove(listenerToRemove); }

		protected:
			ScopedPointer<FlexList<ColourPalette::Node>> colourNodeList;
			ListenerList<Listener> listeners;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourPalette)
		};
	}
}

