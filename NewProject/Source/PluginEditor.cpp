/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    volumeSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    addAndMakeVisible (volumeSlider.get());
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "VOL", *volumeSlider);
    
    volumeLabel = std::make_unique<juce::Label>("", "Volume");
    addAndMakeVisible(volumeLabel.get());
    
    volumeLabel->attachToComponent(volumeSlider.get(), false);
    volumeLabel->setJustificationType(juce::Justification::centred);
    
    //LPF///////////////////////////////
    
    lpfSlider = std::make_unique<juce::Slider>(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    addAndMakeVisible(lpfSlider.get());
    lpfAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LPF", *lpfSlider);
    lpfLabel = std::make_unique<juce::Label>("", "Low-Pass");
    addAndMakeVisible(lpfLabel.get());
    lpfLabel->attachToComponent (lpfSlider.get(), false);
    lpfLabel->setJustificationType(juce::Justification::centred);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    bounds.removeFromTop (40);
    bounds.reduce (40, 40);
    
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;
    using Fr = juce::Grid::Fr;
    
    grid.items.add (juce::GridItem (lpfSlider.get()));
    grid.items.add (juce::GridItem (volumeSlider.get()));
    
    grid.templateColumns = { Track (Fr (1)), Track (Fr (1)), Track (Fr (1)), Track (Fr (1)), };
    grid.templateRows = { Track (Fr (1)), Track (Fr (1)) };
    grid.columnGap = juce::Grid::Px (10);
    grid.rowGap = juce::Grid::Px (10);
    
    grid.performLayout (bounds);
}
