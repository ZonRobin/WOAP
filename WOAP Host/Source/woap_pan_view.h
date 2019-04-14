#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_panner.h"
#include "woap_parameter.h"

namespace WOAP
{
	class ParameterFloat;

	namespace GUI
	{
		class PanView : public Panner, public Parameter::Listener
		{
		public:
			PanView(ParameterFloat* parameter = nullptr);
			virtual ~PanView();

			void setParameter(ParameterFloat* parameter);

			virtual void valueChanged();
			virtual void startedDragging();
			virtual void stoppedDragging();

			virtual void parameterChanged(Parameter*) override;

		protected:
			bool dragging = false;
			ParameterFloat* parameter;

		private:
			JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanView)
		};
	}
}
