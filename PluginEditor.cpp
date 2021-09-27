/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CS_2110AudioProcessorEditor::CS_2110AudioProcessorEditor (CS_2110AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    for( auto* comp : getComps() )
        addAndMakeVisible(comp);
    
    setSize (600, 400);
}

CS_2110AudioProcessorEditor::~CS_2110AudioProcessorEditor()
{
}

//==============================================================================
void CS_2110AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("CS_2110", getLocalBounds(), juce::Justification::centred, 1);
}

void CS_2110AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    auto TopArea = bounds.removeFromTop(bounds.getHeight()*0.20);
    inputGainSlider.setBounds(TopArea.removeFromLeft(TopArea.getWidth()*0.66));
    outputGainSlider.setBounds(TopArea);
    
    auto CutFiltersArea = bounds.removeFromLeft(bounds.getWidth()*0.2);
    auto LC_area = CutFiltersArea.removeFromBottom(CutFiltersArea.getHeight()*0.5);
    
    lowCutFreqSlider.setBounds(LC_area.removeFromBottom(LC_area.getHeight()*0.33));
    lowCutSlope.setBounds(LC_area);
    
    highCutFreqSlider.setBounds(CutFiltersArea.removeFromBottom(CutFiltersArea.getHeight()*0.33));
    highCutSlope.setBounds(CutFiltersArea);
    
    auto LMP_Area = bounds.removeFromLeft(bounds.getWidth()*0.25);
    
    LMP_FreqSlider.setBounds(LMP_Area.removeFromTop(LMP_Area.getHeight()*0.33));
    LMP_GainSlider.setBounds(LMP_Area.removeFromTop(LMP_Area.getHeight()*0.5));
    LMP_QualitySlider.setBounds(LMP_Area);
    
    auto MP_Area = bounds.removeFromLeft(bounds.getWidth()*0.33);
    MP_FreqSlider.setBounds(MP_Area.removeFromTop(MP_Area.getHeight()*0.33));
    MP_GainSlider.setBounds(MP_Area.removeFromTop(MP_Area.getHeight()*0.5));
    MP_QualitySlider.setBounds(MP_Area);
    
    auto HMP_Area = bounds.removeFromLeft(bounds.getWidth()*0.5);
    HMP_FreqSlider.setBounds(HMP_Area.removeFromTop(HMP_Area.getHeight()*0.33));
    HMP_GainSlider.setBounds(HMP_Area.removeFromTop(HMP_Area.getHeight()*0.5));
    HMP_QualitySlider.setBounds(HMP_Area);
    
    thresholdSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.25));
    ratioSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.33));
    attackSlider.setBounds(bounds.removeFromTop(bounds.getHeight()*0.5));
    releaseSlider.setBounds(bounds);
}

std::vector<juce::Component*> CS_2110AudioProcessorEditor::getComps()
{
    return{ &inputGainSlider,
            &lowCutFreqSlider, &lowCutSlope,
            &highCutFreqSlider, &highCutSlope,
            &LMP_FreqSlider, &LMP_GainSlider, &LMP_QualitySlider,
            &MP_FreqSlider, &MP_GainSlider, &MP_QualitySlider,
            &HMP_FreqSlider, &HMP_GainSlider, &HMP_QualitySlider,
            &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider,
            &outputGainSlider };
}
