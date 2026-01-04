#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LifterProcessor::LifterProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ), apvts(*this, nullptr, "Parameters", createParams())
{
}

LifterProcessor::~LifterProcessor()
{
}

//==============================================================================
const juce::String LifterProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LifterProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool LifterProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool LifterProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double LifterProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LifterProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int LifterProcessor::getCurrentProgram()
{
    return 0;
}

void LifterProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String LifterProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void LifterProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// =========== PARAMETER LAYOUT ====================
juce::AudioProcessorValueTreeState::ParameterLayout LifterProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Ratio (:1)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::ratioId,
                                                           Parameters::ratioName,
                                                           juce::NormalisableRange<float>(Parameters::ratioMin, Parameters::ratioMax, 0.1f),
                                                           Parameters::ratioDefault
                                                           )
               );
    
    // Threshold (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::thresId,
                                                           Parameters::thresName,
                                                           juce::NormalisableRange<float>(Parameters::thresMin, Parameters::thresMax, 0.1f),
                                                           Parameters::thresDefault
                                                           )
               );
    
    // Knee Width (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::kneeId,
                                                           Parameters::kneeName,
                                                           juce::NormalisableRange<float>(Parameters::kneeMin, Parameters::kneeMax, 0.1f),
                                                           Parameters::kneeDefault
                                                           )
               );
    
    // Attack Time (ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::attackId,
                                                           Parameters::attackName,
                                                           juce::NormalisableRange<float>(Parameters::attackMin, Parameters::attackMax, 0.1f),
                                                           Parameters::attackDefault
                                                           )
               );
    
    // Release Time (ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::releaseId,
                                                           Parameters::releaseName,
                                                           juce::NormalisableRange<float>(Parameters::releaseMin, Parameters::releaseMax, 0.1f),
                                                           Parameters::releaseDefault
                                                           )
               );
    
    // Make-up Gain (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::makeupId,
                                                           Parameters::makeupName,
                                                           juce::NormalisableRange<float>(Parameters::makeupMin, Parameters::makeupMax, 0.1f),
                                                           Parameters::makeupDefault
                                                           )
               );
    
    // Topology (feedforward / feedback)
    layout.add(std::make_unique<juce::AudioParameterBool>(
                                                          Parameters::feedId,
                                                          Parameters::feedName,
                                                          Parameters::feedDefault
                                                          )
               );
    
    // Mix (%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::mixId,
                                                           Parameters::mixName,
                                                           juce::NormalisableRange<float>(Parameters::mixMin, Parameters::mixMax, 1.0f),
                                                           Parameters::mixDefault
                                                           )
               );
        
    return layout;
}

//==============================================================================
void LifterProcessor::updateParameters()
{
    lifter.updateRatio( apvts.getRawParameterValue(Parameters::ratioId)->load() );
    lifter.updateRange( apvts.getRawParameterValue(Parameters::thresId)->load() );
    lifter.updateKnee( apvts.getRawParameterValue(Parameters::kneeId)->load() );
    
    lifter.updateAttack( apvts.getRawParameterValue(Parameters::attackId)->load() );
    lifter.updateRelease( apvts.getRawParameterValue(Parameters::releaseId)->load() );
    
    lifter.updateMakeUp( apvts.getRawParameterValue(Parameters::makeupId)->load() );
    
    lifter.updateFeedForward ( (bool) apvts.getRawParameterValue(Parameters::feedId)->load() );
    
    lifter.updateMix( apvts.getRawParameterValue(Parameters::mixId)->load() );
}

void LifterProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    lifter.prepare(spec);
    
    updateParameters();
}

void LifterProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool LifterProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void LifterProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear any unused output channel
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Update params
    updateParameters();
    
    // Process
    lifter.process(buffer);
}

//==============================================================================
bool LifterProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LifterProcessor::createEditor()
{
    return new PluginEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void LifterProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void LifterProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LifterProcessor();
}
