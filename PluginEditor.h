/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};

//==============================================================================
/**
*/
class CS_2110AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CS_2110AudioProcessorEditor (CS_2110AudioProcessor&);
    ~CS_2110AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CS_2110AudioProcessor& audioProcessor;

    CustomRotarySlider inputGainSlider,
    lowCutFreqSlider, lowCutSlope, highCutFreqSlider, highCutSlope,
    LMP_FreqSlider, LMP_GainSlider, LMP_QualitySlider,
    MP_FreqSlider, MP_GainSlider, MP_QualitySlider,
    HMP_FreqSlider, HMP_GainSlider, HMP_QualitySlider,
    thresholdSlider, ratioSlider, attackSlider, releaseSlider,
    outputGainSlider;
    
    std::vector<juce::Component*> getComps();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CS_2110AudioProcessorEditor)
};
