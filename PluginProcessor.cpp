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

    juce::dsp::ProcessSpec spec;
    
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;
    spec.sampleRate = sampleRate;
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
    
    auto chainSettings = getChainSettings(apvts);
    
    updateInputGainAndPhase(chainSettings);
    
    updateLowCutFilter(chainSettings);
    updateHighCutFilter(chainSettings);
    
    updateLowPeakFilter(chainSettings);
    updateMediumPeakFilter(chainSettings);
    updateHighPeakFilter(chainSettings);

    updateCompressor(chainSettings);

    updateOutputGain(chainSettings);

}

void CS_2110AudioProcessor::releaseResources()
{
    // When playback sotps, you can use this as an opportunity to free up any
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

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
        
    auto chainSettings = getChainSettings(apvts);
    
    updateInputGainAndPhase(chainSettings);
    
    updateLowCutFilter(chainSettings);
    updateHighCutFilter(chainSettings);
    
    updateLowPeakFilter(chainSettings);
    updateMediumPeakFilter(chainSettings);
    updateHighPeakFilter(chainSettings);

    updateCompressor(chainSettings);

    updateOutputGain(chainSettings);
   
    // Dividiamo dal buffer che l'host invierà il canale destro e sinistro per darli alle rispettive "catene"
    juce::dsp::AudioBlock<float> block(buffer);
    
    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);
    
    //usiamo l'inizializzazione con le parentesi perchè leftContext è una struct e a quanto pare è buona pratica
    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);
    //Ora possiamo passarli e processare l'audio in ingresso
    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
}

//==============================================================================
bool CS_2110AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CS_2110AudioProcessor::createEditor()
{
    return new CS_2110AudioProcessorEditor (*this);
//  return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void CS_2110AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream mos(destData,true);
    apvts.state.writeToStream(mos);
}

void CS_2110AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if(tree.isValid())
    {
        apvts.replaceState(tree);
        updateInputGainAndPhase(getChainSettings(apvts));
        updateCutAndPeakFilters(getChainSettings(apvts));
        updateCompressor(getChainSettings(apvts));
        updateOutputGain(getChainSettings(apvts));
    }
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState & apvts)
{
    ChainSettings settings;
    
    settings.phase = apvts.getRawParameterValue("Phase_Inversion")->load();
    
    settings.inputGainInDecibels = apvts.getRawParameterValue("Input_Gain")->load();

    settings.lowCutFreq = apvts.getRawParameterValue("LowCut_Freq")->load();
    settings.lowCutSlope = static_cast<Slope>( apvts.getRawParameterValue("LowCut_Slope")->load());
    
    settings.highCutFreq = apvts.getRawParameterValue("HighCut_Freq")->load();
    settings.highCutSlope = static_cast<Slope> (apvts.getRawParameterValue("HighCut_Slope")->load());
    
    settings.lowPeakFreq = apvts.getRawParameterValue("LowPeak_Freq")->load();
    settings.lowPeakGainInDecibels = apvts.getRawParameterValue("LowPeak_Gain")->load();
    settings.lowPeakQuality = apvts.getRawParameterValue("LowPeak_Q")->load();
    
    settings.mediumPeakFreq = apvts.getRawParameterValue("MediumPeak_Freq")->load();
    settings.mediumPeakGainInDecibels = apvts.getRawParameterValue("MediumPeak_Gain")->load();
    settings.mediumPeakQuality = apvts.getRawParameterValue("MediumPeak_Q")->load();
    
    settings.highPeakFreq = apvts.getRawParameterValue("HighPeak_Freq")->load();
    settings.highPeakGainInDecibels = apvts.getRawParameterValue("HighPeak_Gain")->load();
    settings.highPeakQuality = apvts.getRawParameterValue("HighPeak_Q")->load();

    settings.thresholdInDecibels = apvts.getRawParameterValue("Threshold")->load();
    settings.ratio = apvts.getRawParameterValue("Ratio")->load();
    settings.attack = apvts.getRawParameterValue("Attack")->load();
    settings.release = apvts.getRawParameterValue("Release")->load();
    //settings.makeUpGainInDecibels = apvts.getRawParameterValue("MakeUp_Gain")->load();
    
    settings.outputGainInDecibels = apvts.getRawParameterValue("Output_Gain")->load();

    return settings;
}

void CS_2110AudioProcessor::updateInputGainAndPhase(const ChainSettings &chainSettings)
{
    auto gain = juce::Decibels::decibelsToGain(chainSettings.inputGainInDecibels);
    
    if(chainSettings.phase)
        gain = -1.f * gain;
    
    leftChain.get<ChainPosition::InputGain>().setGainLinear(gain);
   
    rightChain.get<ChainPosition::InputGain>().setGainLinear(gain);
}

void CS_2110AudioProcessor::updateLowCutFilter(const ChainSettings &chainSettings)
{
    auto lowCutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq,
                                                                                                           getSampleRate(),
                                                                                                           2*(chainSettings.lowCutSlope + 1));
    auto& leftLowCut  = leftChain.get<ChainPosition::LowCut>();
    auto& rightLowCut = rightChain.get<ChainPosition::LowCut>();

       
    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);
    
    rightLowCut.setBypassed<0>(true);
    rightLowCut.setBypassed<1>(true);
    rightLowCut.setBypassed<2>(true);
    rightLowCut.setBypassed<3>(true);
    
    
    switch (chainSettings.lowCutSlope)
    {
        case Slope_48:
            *leftLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            leftLowCut.setBypassed<3>(false);
            *rightLowCut.get<3>().coefficients = *lowCutCoefficients[3];
            rightLowCut.setBypassed<3>(false);
        case Slope_36:
            *leftLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            leftLowCut.setBypassed<2>(false);
            *rightLowCut.get<2>().coefficients = *lowCutCoefficients[2];
            rightLowCut.setBypassed<2>(false);
        case Slope_24:
            *leftLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            leftLowCut.setBypassed<1>(false);
            *rightLowCut.get<1>().coefficients = *lowCutCoefficients[1];
            rightLowCut.setBypassed<1>(false);
        case Slope_12:
            *leftLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            leftLowCut.setBypassed<0>(false);
            *rightLowCut.get<0>().coefficients = *lowCutCoefficients[0];
            rightLowCut.setBypassed<0>(false);
    }
}

void CS_2110AudioProcessor::updateHighCutFilter(const ChainSettings &chainSettings)
{
    auto highCutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(chainSettings.highCutFreq,
                                                                                                           getSampleRate(),
                                                                                                           2*(chainSettings.highCutSlope + 1));
    auto& leftHighCut  = leftChain.get<ChainPosition::HighCut>();
    auto& rightHighCut = rightChain.get<ChainPosition::HighCut>();

       
    leftHighCut.setBypassed<0>(true);
    leftHighCut.setBypassed<1>(true);
    leftHighCut.setBypassed<2>(true);
    leftHighCut.setBypassed<3>(true);
    
    rightHighCut.setBypassed<0>(true);
    rightHighCut.setBypassed<1>(true);
    rightHighCut.setBypassed<2>(true);
    rightHighCut.setBypassed<3>(true);
    
    
    switch (chainSettings.highCutSlope)
    {
        case Slope_48:
            *leftHighCut.get<3>().coefficients = *highCutCoefficients[3];
             leftHighCut.setBypassed<3>(false);
            
            *rightHighCut.get<3>().coefficients = *highCutCoefficients[3];
             rightHighCut.setBypassed<3>(false);
        case Slope_36:
            *leftHighCut.get<2>().coefficients = *highCutCoefficients[2];
             leftHighCut.setBypassed<2>(false);
            
            *rightHighCut.get<2>().coefficients = *highCutCoefficients[2];
             rightHighCut.setBypassed<2>(false);
        case Slope_24:
            *leftHighCut.get<1>().coefficients = *highCutCoefficients[1];
             leftHighCut.setBypassed<1>(false);
            
            *rightHighCut.get<1>().coefficients = *highCutCoefficients[1];
             rightHighCut.setBypassed<1>(false);
        case Slope_12:
            *leftHighCut.get<0>().coefficients = *highCutCoefficients[0];
             leftHighCut.setBypassed<0>(false);
            
            *rightHighCut.get<0>().coefficients = *highCutCoefficients[0];
             rightHighCut.setBypassed<0>(false);
    }
}

void CS_2110AudioProcessor::updateLowPeakFilter(const ChainSettings& chainSettings)
{
    auto lowPeakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                   chainSettings.lowPeakFreq,
                                                                                   chainSettings.lowPeakQuality,
                                                                                   juce::Decibels::decibelsToGain(chainSettings.lowPeakGainInDecibels));
    
    updateCoefficients(leftChain.get<ChainPosition::LowPeak>().coefficients,  lowPeakCoefficients);
    updateCoefficients(rightChain.get<ChainPosition::LowPeak>().coefficients, lowPeakCoefficients);
}

void CS_2110AudioProcessor::updateMediumPeakFilter(const ChainSettings& chainSettings)
{
    auto mediumPeakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                   chainSettings.mediumPeakFreq,
                                                                                   chainSettings.mediumPeakQuality,
                                                                                   juce::Decibels::decibelsToGain(chainSettings.mediumPeakGainInDecibels));
    updateCoefficients(leftChain.get<ChainPosition::MediumPeak>().coefficients,  mediumPeakCoefficients);
    updateCoefficients(rightChain.get<ChainPosition::MediumPeak>().coefficients, mediumPeakCoefficients);


}

void CS_2110AudioProcessor::updateHighPeakFilter(const ChainSettings& chainSettings)
{
    auto highPeakCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(),
                                                                                   chainSettings.highPeakFreq,
                                                                                   chainSettings.highPeakQuality,
                                                                                   juce::Decibels::decibelsToGain(chainSettings.highPeakGainInDecibels));

    updateCoefficients(leftChain.get<ChainPosition::HighPeak>().coefficients,  highPeakCoefficients);
    updateCoefficients(rightChain.get<ChainPosition::HighPeak>().coefficients, highPeakCoefficients);

}

void CS_2110AudioProcessor::updateCutAndPeakFilters(const ChainSettings& chainSettings)
{
    updateLowCutFilter(chainSettings);
    updateHighCutFilter(chainSettings);
    
    updateLowPeakFilter(chainSettings);
    updateMediumPeakFilter(chainSettings);
    updateHighPeakFilter(chainSettings);
}


void CS_2110AudioProcessor::updateCoefficients(Coefficients& old, const Coefficients& replacement) { *old = *replacement;  }


void CS_2110AudioProcessor::updateCompressor(const ChainSettings& chainSetting)
{
    auto& leftComp = leftChain.get<ChainPosition::Compressor>();
    leftComp.setThreshold(chainSetting.thresholdInDecibels);
    leftComp.setRatio(chainSetting.ratio);
    leftComp.setAttack(chainSetting.attack);
    leftComp.setRelease(chainSetting.release);
    
    auto& rightComp = rightChain.get<ChainPosition::Compressor>();
    rightComp.setThreshold(chainSetting.thresholdInDecibels);
    rightComp.setRatio(chainSetting.ratio);
    rightComp.setAttack(chainSetting.attack);
    rightComp.setRelease(chainSetting.release);

}

void CS_2110AudioProcessor::updateOutputGain(const ChainSettings &chainSettings)
{
   
    auto gain = juce::Decibels::decibelsToGain(chainSettings.outputGainInDecibels);

    leftChain.get<ChainPosition::OutputGain>().setGainLinear(gain);
       
    rightChain.get<ChainPosition::OutputGain>().setGainLinear(gain);
}


// Creo la lista con tutti i parametri modificabili dall'utente
juce::AudioProcessorValueTreeState::ParameterLayout CS_2110AudioProcessor::createParameterLayout()
{
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
                                                           juce::NormalisableRange<float>(-60.f, 12.f, 0.5f, 1.f),
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
                                                           juce::NormalisableRange<float>(1000.f, 20000.f, 1.f, 0.5f),
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
                                                           5000.0f));
    
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
                                                           juce::NormalisableRange<float>(-48.f, 0.0f, 0.5f, 1.f),
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

    //layout.add(std::make_unique<juce::AudioParameterFloat>("MakeUp_Gain",
    //                                                       "MakeUp Gain",
    //                                                       juce::NormalisableRange<float>(-12.f, 12.f, 0.05f, 1.f),
    //                                                       0.0f));
    // Output Gain ========================================================================================
    layout.add(std::make_unique<juce::AudioParameterFloat>("Output_Gain",
                                                           "Output Gain",
                                                           juce::NormalisableRange<float>(-60.f, 12.f, 0.5f, 1.f),
                                                           0.0f));
    return layout;
}

//==============================================================================
// This creates new instances of the plugin...
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CS_2110AudioProcessor();
}
