/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Button::Listener,
                                        public juce::Timer
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown (const juce::MouseEvent& e) override;
    
    void buttonClicked (juce::Button* button) override;
    void timerCallback() override;

private:
    
    std::unique_ptr<juce::Slider> volumeSlider, lpfSlider;
    std::unique_ptr<juce::Label> volumeLabel, lpfLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment, lpfAttachment;
    std::unique_ptr<juce::TextButton> lookAndFeelButton;
    
    juce::LookAndFeel_V4 theLFDark, theLFMid, theLFGrey, theLFLight;
    juce::LookAndFeel_V3 theLFV3;
    juce::LookAndFeel_V2 theLFV2;
    int currentLF = 1;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
