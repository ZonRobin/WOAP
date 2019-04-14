#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter_bool.h"

namespace WOAP
{
	class Track;
	class AudioProcessorPlayerDouble;

	namespace GUI
	{
		class ButtonView : public TextButton, public Parameter::Listener
		{
		public:
			ButtonView(const String& text, ParameterBool* parameter = nullptr);
			virtual ~ButtonView();

			void setParameter(ParameterBool* parameter);
			virtual void parameterChanged(Parameter*) override;

			void mouseUp(const MouseEvent&) override;
			virtual void clickedWithoutDrag();

		protected:
			ParameterBool* parameter;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonView)
		};

		class MuteButtonView : public ButtonView
		{
		public:
			MuteButtonView(const String& text, Track* tr = nullptr);
			virtual ~MuteButtonView() {}

			void setTrack(Track* tr);
		};

		class SoloButtonView : public ButtonView
		{
		public:
			SoloButtonView(const String& text, Track* tr = nullptr);
			virtual ~SoloButtonView() {}

			void setTrack(Track* tr);
		};

		class RecordArmButtonView : public ButtonView
		{
		public:
			RecordArmButtonView(const String& text, Track* tr = nullptr);
			virtual ~RecordArmButtonView() {}

			void setTrack(Track* tr);
		};

		class BypassButtonView : public ButtonView
		{
		public:
			BypassButtonView(AudioProcessorPlayerDouble* pl, const String& text);
			virtual ~BypassButtonView() {}

		protected:
			AudioProcessorPlayerDouble* plugin;
		};

		class ActiveButtonView : public ButtonView
		{
		public:
			ActiveButtonView(AudioProcessorPlayerDouble* pl, const String& text);
			virtual ~ActiveButtonView() {}

		protected:
			AudioProcessorPlayerDouble* plugin;
		};
	}
}
