/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ECE484Project1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ECE484Project1AudioProcessorEditor (ECE484Project1AudioProcessor&);
    ~ECE484Project1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ECE484Project1AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ECE484Project1AudioProcessorEditor)
};
