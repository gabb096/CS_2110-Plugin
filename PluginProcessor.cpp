/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CS_2110AudioProcessor::CS_2110AudioProcessor()
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

CS_2110AudioProcessor::~CS_2110AudioProcessor()
{
}

//==============================================================================
const juce::String CS_2110AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CS_2110AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CS_2110AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CS_2110AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CS_2110AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CS_2110AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CS_2110AudioProcessor::getCurrentProgram()
{
    return 0;
}

void CS_2110AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CS_2110AudioProcessor::getProgramName (int index)
{
    return {};
}

void CS_2110AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CS_2110AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void CS_2110AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CS_2110AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CS_2110AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ...do something to the data...
    }
}

//==============================================================================
bool CS_2110AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CS_2110AudioProcessor::createEditor()
{
    //return new CS_2110AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void CS_2110AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CS_2110AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

// Creo la lista con tutti i parametri modificabili dall'utente
juce::AudioProcessorValueTreeState::ParameterLayout CS_2110AudioProcessor::createParameterLayout()
{
    //juce::NormalisableRange<typename ValueType>(rangeStart,  rangeEnd,  intervalValue,  skewFactor, defaultValue)

    juce::StringArray SlopeStrArray;
    for(int i=0; i<4; ++i)
        SlopeStrArray.add( juce::String( (12 + i*12) ) + " dB/Oct" );
    // SlopeStrArray = [12 dB/Oct, 24 dB/Oct, 36 dB/Oct, 48 dB/Oct]
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    // Inversione di fase ==============================================================================
    
    layout.add(std::make_unique<juce::AudioParameterBool>("Phase_Inversion", "Phase", false));
    
    // Input Gain ======================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Input_Gain",
                                                           "Input Gain",
                                                           juce::NormalisableRange<float>(-72.f, 12.f, 0.5f, 1.f),
                                                           0.0f));
    
    // Low cut =========================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCut_Freq",
                                                           "LowCut Freq",
                                                           juce::NormalisableRange<float>(20.f, 2000.f, 1.f, 1.f),
                                                           20.f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut_Slope",
                                                            "LowCut Slope",
                                                            SlopeStrArray,
                                                            0));
    // High cut ========================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCut_Freq",
                                                           "HighCut Freq",
                                                           juce::NormalisableRange<float>(10000.f, 20000.f, 1.f, 1.f),
                                                           20000.f));
    
    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut_Slope",
                                                            "HighCut Slope",
                                                            SlopeStrArray,
                                                            0));

    // Low Medium peak ==================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPeak_Freq",
                                                           "LowPeak Freq",
                                                           juce::NormalisableRange<float>(200.f, 5000.f, 1.f, 1.f),
                                                           1000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPeak_Gain",
                                                           "LowPeak Gain",
                                                           juce::NormalisableRange<float>(-36.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("LowPeak_Q",
                                                           "LowPeak Q",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));
    // Medium peak =======================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("MediumPeak_Freq",
                                                           "MediumPeak Freq",
                                                           juce::NormalisableRange<float>(3000.f, 15000.f, 1.f, 1.f),
                                                           5000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("MediumPeak_Gain",
                                                           "MediumPeak Gain",
                                                           juce::NormalisableRange<float>(-36.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("MediumPeak_Q",
                                                           "MediumPeak Q",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));
    // High Medium peak ====================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPeak_Freq",
                                                           "HighPeak Freq",
                                                           juce::NormalisableRange<float>(7000.f, 20000.f, 1.f, 1.f),
                                                           10000.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPeak_Gain",
                                                           "HighPeak Gain",
                                                           juce::NormalisableRange<float>(-36.f, 24.f, 0.5f, 1.f),
                                                           0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("HighPeak_Q",
                                                           "HighPeak Q",
                                                           juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
                                                           1.f));
    // Compressor =========================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Threshold",
                                                           "Threshold",
                                                           juce::NormalisableRange<float>(-60.f, 0.0f, 0.5f, 1.f),
                                                           0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Ratio",
                                                            "Ratio",
                                                            juce::NormalisableRange<float>(1.f, 20.f, 0.5f, 1.f),
                                                            2.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack",
                                                            "Attack",
                                                            juce::NormalisableRange<float>(1.f, 200.f, 0.5f, 1.f),
                                                            2.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release",
                                                            "Release",
                                                            juce::NormalisableRange<float>(1.f, 200.f, 0.5f, 1.f),
                                                            2.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("MakeUp_Gain",
                                                           "MakeUp Gain",
                                                           juce::NormalisableRange<float>(-12.f, 12.f, 0.05f, 1.f),
                                                           0.0f));

    // Output Gain ========================================================================================
    
    layout.add(std::make_unique<juce::AudioParameterFloat>("Output_Gain",
                                                           "Output Gain",
                                                           juce::NormalisableRange<float>(-72.f, 12.f, 0.5f, 1.f),
                                                           0.0f));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin...
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CS_2110AudioProcessor();
}
