/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

struct LookAndFeel : juce::LookAndFeel_V4
{
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider& ) override;
};

struct RotarySliderWithLabels : juce::Slider
{
    RotarySliderWithLabels(juce::RangedAudioParameter & rap, const juce::String & unitSuffix) :
    juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                 juce::Slider::TextEntryBoxPosition::NoTextBox),
    param(&rap),
    suffix(unitSuffix)
    {
        setLookAndFeel(& lnf);
    }
    
    ~RotarySliderWithLabels()  { setLookAndFeel(nullptr); }
    
    void paint(juce::Graphics& g) override;
    juce::Rectangle<int> getSliderBounds() const;
    int getTextHeight() const { return 14; }
    juce::String getDisplayString() const;
private:
    LookAndFeel lnf;
    
    juce::RangedAudioParameter* param;
    juce::String suffix;
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

    RotarySliderWithLabels inputGainSlider,
    lowCutFreqSlider, lowCutSlopeSlider, highCutFreqSlider, highCutSlopeSlider,
    LMP_FreqSlider, LMP_GainSlider, LMP_QualitySlider,
    MP_FreqSlider, MP_GainSlider, MP_QualitySlider,
    HMP_FreqSlider, HMP_GainSlider, HMP_QualitySlider,
    thresholdSlider, ratioSlider, attackSlider, releaseSlider,
    outputGainSlider;
    
    juce::ToggleButton phaseButton;
    
    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachment = APVTS::SliderAttachment;
    
    Attachment inputGainAttachment,
    lowCutFreqAttachment, lowCutSlope, highCutFreqAttachment, highCutSlopeAttachment,
    LMP_FreqAttachment, LMP_GainAttachment, LMP_QualityAttachment,
    MP_FreqAttachment, MP_GainAttachment, MP_QualityAttachment,
    HMP_FreqAttachment, HMP_GainAttachment, HMP_QualityAttachment,
    thresholdAttachment, ratioAttachment, attackAttachment, releaseAttachment,
    outputGainAttachment;

    juce::ImageComponent imageComponent;

    APVTS::ButtonAttachment phaseButtonAttachment;
    
    std::vector<juce::Component*> getComps();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CS_2110AudioProcessorEditor)
};
