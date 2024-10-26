/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DelayLineEffect.h"

struct Pluginsettings
{
    float LFOfreq{ 0 }; 
    float Delay{ 0 }; 
    float feedforwardGain{ 0 }; 
    float feedbackGain{ 0 };
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
    using Filter = juce::dsp::IIR::Filter<float>;

    using CutFilter =juce::dsp::ProcessorChain<Filter,Filter,Filter,Filter>

    //Monochain
};
