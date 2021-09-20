/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


enum Slope
{
    Slope_12,
    Slope_24,
    Slope_36,
    Slope_48
};

struct ChainSettings
{
    float lowPeakFreq {0}, lowPeakGainInDecibels {0}, lowPeakQuality {1.f};
    float mediumPeakFreq {0}, mediumPeakGainInDecibels {0}, mediumPeakQuality {1.f};
    float highPeakFreq {0}, highPeakGainInDecibels {0}, highPeakQuality {1.f};
    float lowCutFreq {0}, highCutFreq {0};
    int lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
};

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState & apvts);

//==============================================================================
/**
*/
class CS_2110AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    CS_2110AudioProcessor();
    ~CS_2110AudioProcessor() override;

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

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameterLayout()};
    
private:
    
    using Filter = juce::dsp::IIR::Filter<float>; //per fare prima
    
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;//catena di 4 filtri da 12dB/Oct
    //CAPIRE COME AGGIUNGERE GAIN E COMPRESSORE ALLA CATENA ED EVENTUALMENTE CREARNE 2 CON PEAK e COMP INTERCAMBIABILI CON UN BOTTONE
    using MonoChain = juce::dsp::ProcessorChain<CutFilter, CutFilter, Filter, Filter, Filter>; //catena dei 2 filtri cut seguiti dai 3 peck
    
    MonoChain leftChain, rightChain;//catene per i due canali stereo
    
    enum ChainPosition
    {
        LowCut,
        HighCut,
        LowPeak,
        MediumPeak,
        HighPeak
    };
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CS_2110AudioProcessor)
};
