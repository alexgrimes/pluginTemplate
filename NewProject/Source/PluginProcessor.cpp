/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessor::NewProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts (*this, nullptr, "Parameters", createParameters())
#endif
{
    apvts.state.addListener (this);
    init();
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

//==============================================================================
const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
   
    prepare (sampleRate, samplesPerBlock);
    update();
    reset();
    isActive = true;

}

void NewProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    if (!isActive)
    {
        return;
    }
    
    if (mustUpdateProcessing)
    {
        update();
    }
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    auto numSamples = buffer.getNumSamples();
    auto numChannels = juce::jmin (totalNumInputChannels, totalNumOutputChannels);
    
    auto sumMaxVal = 0.0f;
    auto currentMaxVal = meterGlobalMaxVal.load();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);
    
    

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto channelMaxVal = 0.0f;
        
        iirFilter[channel].processSamples(channelData, numSamples);
        
        outputVolume[channel].applyGain (channelData, numSamples);
        
        //absolute value of all samples in a buffer
        //is the current sample larger than the current max?
            //if yes --  channelMaxVal = new max
        
        
        //find max val in this channel / buffer of samples
        
        
        
        for (int sample = 0; sample < numSamples ; ++sample)
        {
            auto rectifiedVal = std::abs (channelData[sample]);
            
            if (channelMaxVal < rectifiedVal)
                channelMaxVal = rectifiedVal;
            
            if (currentMaxVal < rectifiedVal)
                currentMaxVal = rectifiedVal;
        }
        
        sumMaxVal += channelMaxVal; //sum of ch 0 and ch 1 max vals
        
        meterGlobalMaxVal.store (currentMaxVal);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++ sample)
        {
            
            
            //Hard clip values
            channelData[sample] = juce::jlimit (-1.0f, 1.0f, channelData[sample]);
        }

        
    }
    
    meterLocalMaxVal.store (sumMaxVal / (float)numChannels);
}

//==============================================================================
bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}

//==============================================================================
void NewProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    juce::ValueTree copyState = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml = copyState.createXml();
    copyXmlToBinary(*xml.get(), destData);
    
}

void NewProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    std::unique_ptr<juce::XmlElement> xml = getXmlFromBinary(data, sizeInBytes);
    juce::ValueTree copyState = juce::ValueTree::fromXml(*xml.get());
    apvts.replaceState(copyState);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}


void NewProjectAudioProcessor::init()
{
    // Called once - give initial values to DSP
}

void NewProjectAudioProcessor::prepare (double sampleRate, int samplesPerBlock)
{
    // pass to DSP
}

void NewProjectAudioProcessor::update()
{
    //update DSP when user changes params
    
    mustUpdateProcessing = false;
    
    
    auto frequency = apvts.getRawParameterValue("LPF");
    auto volume = apvts.getRawParameterValue("VOL");
    
    
    for (int channel = 0; channel < 2; ++channel)
    {
        iirFilter[channel].setCoefficients (juce::IIRCoefficients::makeLowPass(getSampleRate(), frequency->load()));
        outputVolume[channel].setTargetValue (juce::Decibels::decibelsToGain (volume->load()));
    }
}

void NewProjectAudioProcessor::reset()
{
    //reset DSP params
    
    for (int channel = 0; channel < 2; ++channel)
    {
        iirFilter[channel].reset();
        outputVolume[channel].reset (getSampleRate(), 0.050);
    }
    
    meterLocalMaxVal.store (0.0f);
    meterGlobalMaxVal.store (0.0f);
}

//void NewProjectAudioProcessor::userChangedParameter()
//{
//    //reset DSP params
//
//}

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    
    //create our paramters
    //add them to the vector
    
    std::function<juce::String(float, int)> valueToTextFunction = [](float x, int l) { return juce::String(x, 4); };
    std::function<float(const juce::String&)> textToValueFunction = [](const juce::String& str) { return str.getFloatValue(); };
    
    auto gainParam = std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", juce::NormalisableRange< float > (-40.0f, 40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction);
    
    //filter/////////////////////
    parameters.push_back (std::make_unique<juce::AudioParameterFloat>("LPF", "Low Pass Filter", juce::NormalisableRange<float> (20.0f, 22000.0f, 10.0f, 0.2f), 800.0f, "Hz", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    
    parameters.push_back (std::make_unique<juce::AudioParameterFloat>("VOL", "Volume", juce::NormalisableRange< float > (-40.0f, 40.0f), 0.0f, "dB", juce::AudioProcessorParameter::genericParameter, valueToTextFunction, textToValueFunction));
    
    
    
    return { parameters.begin(), parameters.end() };
}
