#include "DelayLineEffect.h"
/*
This is the user defined code for programming the delay line effects
*/
void DelayLineEffect::ProcessAudio(double samplerate, Pluginsettings CurrentSettings, juce::AudioBuffer<float>& buffer, int channel) {
	
	auto* channelData = buffer.getWritePointer(channel);

	for (int i = 0; i < (buffer.getNumSamples()); i++) {


	}


}