#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_parameter.h"

namespace WOAP
{
	class ParameterBool;
	class ParameterFloat;
	class Track;
	class DCAController;
	class SoloStateDCA;
	class MuteStateDCA;

	class DCA : public Parameter::Listener
	{
	public:
		DCA();
		virtual ~DCA();

		ParameterBool* getMuteParameter() const;
		ParameterBool* getSoloParameter() const;
		ParameterFloat* getGainParameter() const;

		virtual bool getMuteState() const;
		virtual bool getSoloState() const;
		virtual float getGainState() const;

		virtual void assignToController(DCAController* controller);
		virtual void unassignFromController(DCAController* controller);
		virtual void unassignFromAllControllers();

		bool isAssignedToController(DCAController* controller);
		const Array<DCAController*, CriticalSection>& getAssignedControllers() const;

		virtual void parameterChanged(Parameter*) override;

	protected:
		CriticalSection lock;

		ScopedPointer<ParameterBool> muteParameter;
		ScopedPointer<ParameterBool> soloParameter;
		ScopedPointer<ParameterFloat> gainParameter;
		Array<DCAController*, CriticalSection> assignedControllers;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DCA)
	};

	class MuteStateDCA : public DCA
	{
	public:
		MuteStateDCA();
		virtual ~MuteStateDCA();

		virtual bool getMuteState() const;
		virtual bool getSoloState() const;
		virtual float getGainState() const;

		void setDisabled(bool newDisabled);

		virtual void assignToController(DCAController* controller) override;
		virtual void unassignFromController(DCAController* controller) override;

		virtual void parameterChanged(Parameter*) override;

	protected:
		bool disabled = false;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MuteStateDCA)
	};

	class SoloStateDCA : public DCA
	{
	public:
		SoloStateDCA(MuteStateDCA* muteDCA);
		virtual ~SoloStateDCA();

		virtual bool getMuteState() const;
		virtual bool getSoloState() const;
		virtual float getGainState() const;

		virtual void assignToController(DCAController* controller) override;
		virtual void unassignFromController(DCAController* controller) override;

		virtual void parameterChanged(Parameter*) override;
		
	protected:
		MuteStateDCA* muteStateDCA;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoloStateDCA)
	};
}
