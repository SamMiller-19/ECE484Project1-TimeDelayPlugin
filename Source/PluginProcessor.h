/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#define _USE_MATH_DEFINES

#include <JuceHeader.h>
#include "DelayLineEffect.h"
#include <cmath>
enum {
    Sine = false,
    Noise = true
};

struct Pluginsettings
{
    float LFOfreq{ 0 }; 
    float LFOmag{ 0 };
    float delay{ 0 }; 
    float delayGain{ 0 }; 
    float feedbackGain{ 0 };
    float dryGain{ 0 };
    bool delayType{ Sine };

};

Pluginsettings getPluginSettings(juce::AudioProcessorValueTreeState& layout);


//==============================================================================
/**
*/
class ECE484Project1AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ECE484Project1AudioProcessor();
    ~ECE484Project1AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout
        createParamaterLayout();

    juce::AudioProcessorValueTreeState layout{*this, nullptr, "Paramaters", createParamaterLayout()};

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ECE484Project1AudioProcessor)

    /* Here I should declair chains based on the DSP that I'm going to be doing*/
    juce::AudioBuffer<float> delayBuffer;
    int writePosition{ 0 };

    //Value to track the phase of the LFO sin wave
    double sinphase{ 0 };

    //Userdefined function for circular buffers
    void ECE484Project1AudioProcessor::updateCircBuffer(int channel, juce::AudioBuffer<float>& buffer);

    //User Defined function to interpolate between 2 values
    float ECE484Project1AudioProcessor::readInterpolatedValue(float sample, juce::AudioBuffer<float>& buffer, int channel);
    
    
    
    
    
};
