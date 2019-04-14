#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "woap_audio_buffer_extended.h"
#include "woap_identifiable.h"
#include "woap.h"
#include "woap_api_consumer.h"
#include "woap_stateful.h"

namespace WOAP
{
	class ParameterBool;
	class Track;

	class AudioProcessorPlayerDouble : public AudioPlaybackInfoListener, public MidiInputCallback, public Identifiable, public API::APIConsumer, public AudioProcessorListener, public Stateful
	{
	public:
		enum MessageType
		{
			GetDetail,
			GetParameterValues,
			GetParameters,
			SetName,
			SetActive,
			SetBypass,
			SetParameterValue,
			AddListener,
			RemoveListener,
			AddParameterListener,
			RemoveParameterListener,
			AddBypassListener,
			RemoveBypassListener,
			AddActiveListener,
			RemoveActiveListener
		};

		class Listener
		{
		public:
			virtual void nameChanged(AudioProcessorPlayerDouble* plugin) = 0;
			virtual void aboutToDelete(AudioProcessorPlayerDouble* plugin) = 0;
		};

		class ParameterListener
		{
		public:
			virtual void audioProcessorParameterChanged(AudioProcessorPlayerDouble* plugin, int parameterIndex, float newValue) = 0;
			virtual void audioProcessorParameterChangeGestureBegin(AudioProcessorPlayerDouble* plugin, int parameterIndex) = 0;
			virtual void audioProcessorParameterChangeGestureEnd(AudioProcessorPlayerDouble* plugin, int parameterIndex) = 0;
			virtual void aboutToDelete(AudioProcessorPlayerDouble* plugin) = 0;
		};

		struct ParameterInfo
		{
			ParameterInfo(int paramIndex, const String& paramRef, float paramValue, const String& paramValueText)
				: index(paramIndex), ref(paramRef), value(paramValue), valueText(paramValueText) {}

			int index;
			String ref;
			float value;
			String valueText;
		};

		AudioProcessorPlayerDouble(Track* tr, AudioPluginInstance* processorToPlay, bool doDoublePrecisionProcessing = true);
		AudioProcessorPlayerDouble(const Uuid& idToUse, Track* tr, AudioPluginInstance* processorToPlay, bool doDoublePrecisionProcessing = true);
		virtual ~AudioProcessorPlayerDouble();
				
		AudioProcessorEditor* createEditor() const;

		Track* getTrack() const { return track; }

		String getName() const { return name; }
		void setName(const String& newName);

		String getType() const;
		int getTypeId() const;

		void setLastPluginEditorPosition(Point<int> newPosition) { lastPluginEditorPosition = newPosition; }
		Point<int> getLastPluginEditorPosition() const { return lastPluginEditorPosition; }

		ParameterBool* getBypassParameter() const { return bypassParameter; }
		ParameterBool* getActiveStateParameter() const { return activeStateParameter; }

		void processBlock(const AudioBufferDouble& inputBuffer, AudioBufferDouble& outputBuffer);

		/** Returns the current audio processor that is being played. */
		AudioPluginInstance* getCurrentProcessor() const noexcept { return processor; }

		/** Returns a midi message collector that you can pass midi messages to if you
		want them to be injected into the midi stream that is being sent to the
		processor.
		*/
		MidiMessageCollector& getMidiMessageCollector() noexcept { return messageCollector; }

		/** Switch between double and single floating point precisions processing.
		The audio IO callbacks will still operate in single floating point
		precision, however, all internal processing including the
		AudioProcessor will be processed in double floating point precision if
		the AudioProcessor supports it (see
		AudioProcessor::supportsDoublePrecisionProcessing()).
		Otherwise, the processing will remain single precision irrespective of
		the parameter doublePrecision. */
		void setDoublePrecisionProcessing(bool doublePrecision);

		/** Returns true if this player processes internally processes the samples with
		double floating point precision. */
		inline bool getDoublePrecisionProcessing() { return isDoublePrecision; }

		virtual void playbackAboutToStart(double newSampleRate, int newBufferSizeSamples) override;
		virtual void playbackAboutToStop() override;
		/** @internal */
		void handleIncomingMidiMessage(MidiInput*, const MidiMessage&) override;
		
		void addListener(Listener* listener) { listeners.add(listener); }
		void removeListener(Listener* listener) { listeners.remove(listener); }

		void addParameterListener(ParameterListener* listener) { parameterListeners.add(listener); }
		void removeParameterListener(ParameterListener* listener) { parameterListeners.remove(listener); }

		virtual Array<API::AddressWithTypeId> getMessageTypesToListenTo() const;

		virtual void messageReceived(API::OSC::RemoteClient& client, const OSCMessage& message, int messageTypeId) override;

		void setParameter(int parameterIndex, float newValue);
		void setParameterNotifyingHost(int parameterIndex, float newValue);

		virtual void audioProcessorParameterChanged(AudioProcessor* processor, int parameterIndex, float newValue) override;
		virtual void audioProcessorChanged(AudioProcessor*) override {}
		virtual void audioProcessorParameterChangeGestureBegin(AudioProcessor* processor, int parameterIndex) override;
		virtual void audioProcessorParameterChangeGestureEnd(AudioProcessor* processor, int parameterIndex) override;

		Array<ParameterInfo> getParametersInfo() const;
		Array<float> getParameterValues() const;

		virtual ValueTree getState() const override;
		virtual void setState(const ValueTree& state) override;
		static AudioProcessorPlayerDouble* fromState(Track* track, const ValueTree& state, String& errorMsg);

	protected:
		/** Sets the processor that should be played.

		The processor that is passed in will be deleted and owned by this object.
		To stop anything playing, pass a nullptr to this method.
		*/
		void setProcessor(AudioPluginInstance* processorToPlay);
		void fillBypassSamples(AudioBufferDouble& outputBuffer);

		Track* track;
		String name;
		Point<int> lastPluginEditorPosition;

		AudioPluginInstance* processor = nullptr;
		CriticalSection lock;
		double sampleRate = 0;
		int blockSize = 0;
		bool isPrepared = false;
		bool isDoublePrecision;
		
		NumChannels numMainBusInputChannels = NumChannels::None;
		NumChannels numMainBusOutputChannels = NumChannels::None;

		int numTotalInputChannels = 0;
		int numTotalOutputChannels = 0;
		
		AudioBufferDouble tempBuffer;
		AudioBufferFloat conversionBuffer;
		AudioBufferDouble bypassDelayBuffer;

		MidiBuffer incomingMidi;
		MidiMessageCollector messageCollector;

		ListenerList<Listener> listeners;
		ListenerList<ParameterListener> parameterListeners;

		ScopedPointer<ParameterBool> bypassParameter;
		ScopedPointer<ParameterBool> activeStateParameter;

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorPlayerDouble)
	};
}
