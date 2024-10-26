
#include <JuceHeader.h>
#include "PluginProcessor.h"

class DelayLineEffect
{
public:

	static void ProcessAudio(double samplerate, Pluginsettings CurrentSettings, juce::AudioBuffer<float>& buffer, int channel);

};