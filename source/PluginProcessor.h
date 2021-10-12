/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


enum Slope
{
    Slope_6,
    Slope_12,
    Slope_18,
    Slope_24
};

struct ChainSettings
{
    //controllo di fase
    bool phase {false};
    //input e output gain
    float inputGainInDecibels{0}, outputGainInDecibels{0};
    // lowCut e highCut
    float lowCutFreq {0}, highCutFreq {0};
    int lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_6};
    // filtri peak
    float lowPeakFreq {0}, lowPeakGainInDecibels {0}, lowPeakQuality {1.f};
    float mediumPeakFreq {0}, mediumPeakGainInDecibels {0}, mediumPeakQuality {1.f};
    float highPeakFreq {0}, highPeakGainInDecibels {0}, highPeakQuality {1.f};
    // compressore
    float thresholdInDecibels{0}, ratio{1.f}, attack{1.f}, release{1.f};
    
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
    
    using Gain = juce::dsp::Gain<float>;
    
    using Filter = juce::dsp::IIR::Filter<float>; //per fare prima
    
    using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;
    //catena di 4 filtri da -6dB/Oct
   
    using Comp = juce::dsp::Compressor<float>;

    using MonoChain = juce::dsp::ProcessorChain<Gain, CutFilter, CutFilter, Filter, Filter, Filter, Comp, Gain>;
    
    MonoChain leftChain, rightChain;//catene per i due canali stereo
    
    void updateInputGainAndPhase(const ChainSettings &chainSettings);
    
    void updateLowCutFilter(const ChainSettings& chainSettings);
    void updateHighCutFilter(const ChainSettings& chainSettings);
    
    void updateLowPeakFilter(const ChainSettings& chainSetting);
    void updateMediumPeakFilter(const ChainSettings& chainSetting);
    void updateHighPeakFilter(const ChainSettings& chainSetting);

    void updateCutAndPeakFilters(const ChainSettings& chainSetting);
    
    void updateCompressor(const ChainSettings& chainSetting);
    
    void updateOutputGain(const ChainSettings &chainSettings);

    using Coefficients = Filter::CoefficientsPtr;
    
    static void updateCoefficients(Coefficients& old, const Coefficients& replacement);

    enum ChainPosition
    {
        InputGain,
        LowCut,
        HighCut,
        LowPeak,
        MediumPeak,
        HighPeak,
        Compressor,
        OutputGain
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CS_2110AudioProcessor)
};
