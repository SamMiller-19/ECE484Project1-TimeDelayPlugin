/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ECE484Project1AudioProcessor::ECE484Project1AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

ECE484Project1AudioProcessor::~ECE484Project1AudioProcessor()
{
}

//==============================================================================
const juce::String ECE484Project1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ECE484Project1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ECE484Project1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ECE484Project1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ECE484Project1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ECE484Project1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ECE484Project1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void ECE484Project1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ECE484Project1AudioProcessor::getProgramName (int index)
{
    return {};
}

void ECE484Project1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void ECE484Project1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto delaybuffersize = sampleRate * 2.0;
    delayBuffer.setSize(getTotalNumInputChannels(), (int)delaybuffersize);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;

    auto CurrentSettings = getPluginSettings(layout);

    

}

void ECE484Project1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ECE484Project1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ECE484Project1AudioProcessor::updateCircBuffer(int channel,juce::AudioBuffer<float>& buffer) {

    int delayBufferSize = delayBuffer.getNumSamples();
    int bufferSize = buffer.getNumSamples();

    if (writePosition + bufferSize < delayBufferSize) {
        delayBuffer.copyFrom(channel, writePosition, buffer, channel,0, bufferSize);
        writePosition += bufferSize;
    }
    else {
        delayBuffer.copyFrom(channel, writePosition, buffer, channel, 0, (delayBufferSize - writePosition));

        writePosition += bufferSize;
        writePosition %= delayBufferSize;

        delayBuffer.copyFrom(channel, 0, buffer, channel, bufferSize - writePosition, writePosition);

    }
    
}

//read back the interpolated value
float ECE484Project1AudioProcessor::readInterpolatedValue(double sample, juce::AudioBuffer<float>& buffer, int channel) {
    int bufferSize = buffer.getNumSamples();
    auto* channelData = delayBuffer.getWritePointer(channel);
    //find the lower whole number
    int whole = trunc(sample);
    //Find the fractional value
    double frac = sample - whole;
    float value;

    if (whole < bufferSize-1) {
        value = (1 - frac) * channelData[whole] + frac * channelData[whole + 1];
    }
    else {
        value = (1 - frac) * channelData[whole]+frac * channelData[0];
    }

    return value;

}


void ECE484Project1AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    int delayBufferSize = delayBuffer.getNumSamples();
    int bufferSize = buffer.getNumSamples();
    

    double samplerate = getSampleRate();
    //to speed up computation
    double samplePeriod = 1 / samplerate;
    
    //Get the current settings from the GUI
    auto CurrentSettings = getPluginSettings(layout);
    if (CurrentSettings.LFOmag > CurrentSettings.delay) {

        CurrentSettings.LFOmag = CurrentSettings.delay;
        //Ideally update the slider to reflect this but oh well
    }

    //Convert relavent data to sample time instead of delay time
    double LFOfreqSamples = CurrentSettings.LFOfreq / samplerate;

    double LFOmagSamples = samplerate * CurrentSettings.LFOmag;
    double delaySamples = samplerate * CurrentSettings.delay/1000;
    

    //temporary sinephase for later
    int tempWritePosition;
    double tempSinPhase;


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        //Clear extra input channels
        buffer.clear (i, 0, buffer.getNumSamples());


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //This is the circular buffer that will store all the looped back input data
        auto* inputData =  delayBuffer.getWritePointer(channel);
        //This data will be copied into the circular buffer then will be modified for output
        auto* outputData = buffer.getWritePointer(channel);
                
        tempSinPhase = sinphase;
        tempWritePosition = writePosition;
        //Update the circular buffer, note this updates the write position so we use tempWrite Position instead
        updateCircBuffer(channel, buffer);
        

        for (int sample = 0; sample < (buffer.getNumSamples()); sample++) {
            

            double delayedPosition =(double)tempWritePosition - delaySamples + LFOmagSamples * sin(tempSinPhase * LFOmagSamples);
            
            if (delayedPosition < 0) {
                //If it's 0 make sure to wrap around
                delayedPosition = delayBufferSize + delayedPosition;
            }
            else if (delayedPosition > delayBufferSize) {
                delayedPosition = delayedPosition - delayBufferSize;
            }

            //We can now find the after write values
            float AfterDelay = readInterpolatedValue(delayedPosition, delayBuffer, channel);

            //Now we can calculate the input to the delay signal, note this is actually the input so we just
            //Put it back into the channel Data, the sqrt function is to normalize it so we don't have massive gain
            inputData[tempWritePosition] += CurrentSettings.feedbackGain * AfterDelay;

            //With this we can now actually Calculate the output signal
            outputData[sample] = CurrentSettings.delayGain * AfterDelay + CurrentSettings.dryGain * inputData[tempWritePosition];


            


            //Increment Write Position and the samplePeriod
            tempSinPhase += M_PI * samplePeriod;
            tempWritePosition++;
            
        }
        
        

        // ..do something to the data...
    }
    //If we're done the for loop this is the true sinphase, but it should only be updated on the last itteration
    sinphase = tempSinPhase;
    if (sinphase > 2 * M_PI) {
        sinphase = sinphase - 2 * M_PI;
    }


}

//==============================================================================
bool ECE484Project1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ECE484Project1AudioProcessor::createEditor()
{
    //return new ECE484Project1AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ECE484Project1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ECE484Project1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

Pluginsettings getPluginSettings(juce::AudioProcessorValueTreeState& layout) {

    Pluginsettings settings;
    settings.LFOfreq = layout.getRawParameterValue("LFO Freq")->load();
    settings.LFOmag = layout.getRawParameterValue("LFO Magnitude")->load();
    settings.delay = layout.getRawParameterValue("Delay")->load();
    settings.delayGain = layout.getRawParameterValue("Delay Gain")->load();
    settings.feedbackGain = layout.getRawParameterValue("Feedback Gain")->load();
    settings.dryGain = layout.getRawParameterValue("Dry Gain")->load();
    settings.delayType = layout.getParameter("LFO Type");

    
    return settings;
}

//Set the layout of sliders by creating layout

juce::String BooltoString(bool value){
    if (value == 0) {
        return "Sinusoidal";

    }
    else
        return "White Noise";
}

juce::AudioProcessorValueTreeState::ParameterLayout
ECE484Project1AudioProcessor::createParamaterLayout() {
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "LFO Freq",
        "LFO Freq",
        juce::NormalisableRange<float>(0.0f,10.f,0.1f),
        0.0f));
    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "LFO Magnitude",
        "LFO Magnitude in ms",
        juce::NormalisableRange<float>(0.0f, 1000.f, 10.f),
        0.0f));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "Delay",
        "Delay in ms",
        juce::NormalisableRange<float>(0.0f, 1000.f, 10.f), 
        0.0f));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "Delay Gain",
        "Delay Gain",
        juce::NormalisableRange<float>(0.0f, 1.f, 0.05f),
        0.0f));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "Feedback Gain",
        "Feedback Gain",
        juce::NormalisableRange<float>(0.0f, 1.f, 0.05f), 
        0.0f));

    layout.add(std::make_unique <juce::AudioParameterFloat>(
        "Dry Gain",
        "Dry Gain",
        juce::NormalisableRange<float>(0.0f, 1.f, 0.05f),
        0.0f));
    
    layout.add(std::make_unique <juce::AudioParameterBool>
        ("LFO Type",
            "Check for White Noise",
            0));
        
    return layout;

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ECE484Project1AudioProcessor();
}
