/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class NewProjectAudioProcessor  : public juce::AudioProcessor,
                                  public juce::ValueTree::Listener
{
public:
    //==============================================================================
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

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
    
    //==============================================================================
    
    void init(); // Called once - give initial values to DSP
    void prepare (double sampleRate, int samplesPerBlock); // pass to DSP
    void update(); //update DSP when user changes params
    void reset() override; //reset DSP params
    
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    


private:
    bool mustUpdateProcessing { false };
    bool isActive { false };
    
    void valueTreePropertyChanged (juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property) override
    {
            //detect when a user changes params
        mustUpdateProcessing = true;
    }
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)
};
