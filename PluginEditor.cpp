/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

/* palette colori usati
    giallo Colour(255u, 191u,0u)
    arancio Colour(255u, 165u, 0u)
    blu scuro Colour(67u, 69u, 90u)
    menta Colour(171u, 229u, 213u)

 */

#include "PluginProcessor.h"
#include "PluginEditor.h"

void LookAndFeel::drawRotarySlider(juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPosProportional,
                                   float rotaryStartAngle,
                                   float rotaryEndAngle,
                                   juce::Slider& slider)
{
    using namespace juce;

    auto bounds = Rectangle<float>(x, y, width, height);
    
    g.setColour(Colour(255u, 191u,0u)); //colore del bordo
    //g.setColour(Colours::green);
    g.drawEllipse(bounds,3.f);  //disegno bordo
    
    if(auto* rswl = dynamic_cast<RotarySliderWithLabels*>(&slider))
    {
        auto center = bounds.getCentre();
        Path p;
        
        Rectangle<float> r;
        r.setLeft(center.getX()-2);
        r.setRight(center.getX()+2);
        r.setTop(bounds.getY()+7);
        r.setBottom(center.getY()*0.9);
        
        p.addRoundedRectangle(r,3.f);
        
        jassert(rotaryStartAngle < rotaryEndAngle);
        
        auto sliderAngRad = jmap(sliderPosProportional, 0.f, 1.f, rotaryStartAngle, rotaryEndAngle);
        
        p.applyTransform(AffineTransform().rotated(sliderAngRad, center.getX(), center.getY()));
        
        g.fillPath(p);
        
        }
   
}

void RotarySliderWithLabels::paint(juce:: Graphics &g)
{
    using namespace juce;
    auto startAng = degreesToRadians(225.f);
    auto endAng = degreesToRadians(135.f) + MathConstants<float>::twoPi;
    
    auto range = getRange();
    
    auto sliderBounds = getSliderBounds();

    getLookAndFeel().drawRotarySlider(g,
                                      sliderBounds.getX(),
                                      sliderBounds.getY(),
                                      sliderBounds.getWidth(),
                                      sliderBounds.getHeight(),
                                      jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0),
                                      startAng,
                                      endAng,
                                      *this);
}

juce::Rectangle<int> RotarySliderWithLabels::getSliderBounds() const
{
    
    auto bounds = getLocalBounds();
    
    auto size = juce::jmin( bounds.getWidth(), bounds.getHeight() );
    size -= getTextHeight() * 2;
    
    juce::Rectangle<int> r;
    r.setSize(size, size);
    r.setCentre(bounds.getCentreX(), 0);
    r.setY(10);
    
    return r;
}

juce::String RotarySliderWithLabels::getDisplayString() const
{
    return juce::String(getValue());
}

//==============================================================================
CS_2110AudioProcessorEditor::CS_2110AudioProcessorEditor (CS_2110AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
inputGainSlider(*audioProcessor.apvts.getParameter("Input_Gain"), "dB"),
lowCutFreqSlider(*audioProcessor.apvts.getParameter("LowCut_Freq"), "Hz"),
lowCutSlopeSlider(*audioProcessor.apvts.getParameter("LowCut_Slope"), "dB/Oct"),
highCutFreqSlider(*audioProcessor.apvts.getParameter("HighCut_Freq"), "Hz"),
highCutSlopeSlider(*audioProcessor.apvts.getParameter("HighCut_Slope"), "dB/Oct"),
LMP_FreqSlider(*audioProcessor.apvts.getParameter("LowPeak_Freq"), "Hz"),
LMP_GainSlider(*audioProcessor.apvts.getParameter("LowPeak_Gain"), "dB"),
LMP_QualitySlider(*audioProcessor.apvts.getParameter("LowPeak_Q"), ""),
MP_FreqSlider(*audioProcessor.apvts.getParameter("MediumPeak_Freq"), "Hz"),
MP_GainSlider(*audioProcessor.apvts.getParameter("MediumPeak_Gain"), "dB"),
MP_QualitySlider(*audioProcessor.apvts.getParameter("MediumPeak_Q"), ""),
HMP_FreqSlider(*audioProcessor.apvts.getParameter("HighPeak_Freq"), "Hz"),
HMP_GainSlider(*audioProcessor.apvts.getParameter("HighPeak_Gain"), "dB"),
HMP_QualitySlider(*audioProcessor.apvts.getParameter("HighPeak_Q"), ""),
thresholdSlider(*audioProcessor.apvts.getParameter("Threshold"), "dB"),
ratioSlider(*audioProcessor.apvts.getParameter("Ratio"), ""),
attackSlider(*audioProcessor.apvts.getParameter("Attack"), "ms"),
releaseSlider(*audioProcessor.apvts.getParameter("Release"), "ms"),
outputGainSlider(*audioProcessor.apvts.getParameter("Output_Gain"), "dB"),
phaseButton("Phase"),
//===================================================================
inputGainAttachment(audioProcessor.apvts, "Input_Gain", inputGainSlider),
lowCutFreqAttachment(audioProcessor.apvts, "LowCut_Freq", lowCutFreqSlider),
lowCutSlope(audioProcessor.apvts, "LowCut_Slope", lowCutSlopeSlider),
highCutFreqAttachment(audioProcessor.apvts, "HighCut_Freq", highCutFreqSlider),
highCutSlopeAttachment(audioProcessor.apvts, "HighCut_Slope", highCutSlopeSlider),
LMP_FreqAttachment(audioProcessor.apvts, "LowPeak_Freq", LMP_FreqSlider),
LMP_GainAttachment(audioProcessor.apvts, "LowPeak_Gain", LMP_GainSlider),
LMP_QualityAttachment(audioProcessor.apvts, "LowPeak_Q", LMP_QualitySlider),
MP_FreqAttachment(audioProcessor.apvts, "MediumPeak_Freq", MP_FreqSlider),
MP_GainAttachment(audioProcessor.apvts, "MediumPeak_Gain", MP_GainSlider),
MP_QualityAttachment(audioProcessor.apvts, "MediumPeak_Q", MP_QualitySlider),
HMP_FreqAttachment(audioProcessor.apvts, "HighPeak_Freq", HMP_FreqSlider),
HMP_GainAttachment(audioProcessor.apvts, "HighPeak_Gain", HMP_GainSlider),
HMP_QualityAttachment(audioProcessor.apvts, "HighPeak_Q", HMP_QualitySlider),
thresholdAttachment(audioProcessor.apvts, "Threshold", thresholdSlider),
ratioAttachment(audioProcessor.apvts, "Ratio", ratioSlider),
attackAttachment(audioProcessor.apvts, "Attack", attackSlider),
releaseAttachment(audioProcessor.apvts, "Release", releaseSlider),
outputGainAttachment(audioProcessor.apvts, "Output_Gain", outputGainSlider),
phaseButtonAttachment(audioProcessor.apvts, "Phase_Inversion", phaseButton)
{
    auto bgImage = juce::ImageCache::getFromMemory(BinaryData::BackGround_png, BinaryData::BackGround_pngSize);
    
    imageComponent.setImage(bgImage);
    addAndMakeVisible(imageComponent);
    
    addAndMakeVisible(phaseButton);
    phaseButton.setClickingTogglesState(true);

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
    using namespace juce;

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CS_2110AudioProcessorEditor::resized()
{
    
    //imageComponent.setBounds(getLocalBounds());
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    auto bigKnob = width*0.13;
    auto smallKnob = width*0.12;
    auto h = height/10;
    auto w = width/15;
    
    inputGainSlider.setBounds  (2*w-bigKnob/2, h-bigKnob/2, bigKnob, bigKnob);
    
    phaseButton.setBounds(150, 15, 25, 30);
    
    outputGainSlider.setBounds(13*w-bigKnob/2, h-bigKnob/2, bigKnob, bigKnob);
    
    lowCutFreqSlider.setBounds (2*w-bigKnob/2, 3*h-bigKnob/2, bigKnob, bigKnob);
    lowCutSlopeSlider.setBounds(3*w-smallKnob/2, 5*h-smallKnob/2, smallKnob, smallKnob);
    
    highCutFreqSlider.setBounds( 2*w-bigKnob/2, 7*h-bigKnob/2, bigKnob, bigKnob);
    highCutSlopeSlider.setBounds(3*w-smallKnob/2, 9*h-smallKnob/2, smallKnob, smallKnob);
    
    
    LMP_FreqSlider.setBounds   (5*w-bigKnob/2, 3*h-bigKnob/2, bigKnob, bigKnob);
    LMP_GainSlider.setBounds   (5*w-bigKnob/2, 5.5*h-bigKnob/2, bigKnob, bigKnob);
    LMP_QualitySlider.setBounds(5*w-bigKnob/2, 8*h-bigKnob/2, bigKnob, bigKnob);
    
    MP_FreqSlider.setBounds   (7.5*w-bigKnob/2, 3*h-bigKnob/2, bigKnob, bigKnob);
    MP_GainSlider.setBounds   (7.5*w-bigKnob/2, 5.5*h-bigKnob/2, bigKnob, bigKnob);
    MP_QualitySlider.setBounds(7.5*w-bigKnob/2, 8*h-bigKnob/2, bigKnob, bigKnob);
    
    HMP_FreqSlider.setBounds   (10*w-bigKnob/2, 3*h-bigKnob/2, bigKnob, bigKnob);
    HMP_GainSlider.setBounds   (10*w-bigKnob/2, 5.5*h-bigKnob/2, bigKnob, bigKnob);
    HMP_QualitySlider.setBounds(10*w-bigKnob/2, 8*h-bigKnob/2, bigKnob, bigKnob);
    
    thresholdSlider.setBounds(13*w-bigKnob/2, 3*h-bigKnob/2, bigKnob, bigKnob);
    ratioSlider.setBounds    (13*w-bigKnob/2, 5*h-bigKnob/2, bigKnob, bigKnob);
    attackSlider.setBounds   (12.3*w-smallKnob/2, 7*h-smallKnob/2, smallKnob, smallKnob);
    releaseSlider.setBounds  (13.7*w-smallKnob/2, 9*h-smallKnob/2, smallKnob, smallKnob);
    
}

std::vector<juce::Component*> CS_2110AudioProcessorEditor::getComps()
{
    return{ &inputGainSlider,
            &lowCutFreqSlider, &lowCutSlopeSlider,
            &highCutFreqSlider, &highCutSlopeSlider,
            &LMP_FreqSlider, &LMP_GainSlider, &LMP_QualitySlider,
            &MP_FreqSlider, &MP_GainSlider, &MP_QualitySlider,
            &HMP_FreqSlider, &HMP_GainSlider, &HMP_QualitySlider,
            &thresholdSlider, &ratioSlider, &attackSlider, &releaseSlider,
            &outputGainSlider };
}
