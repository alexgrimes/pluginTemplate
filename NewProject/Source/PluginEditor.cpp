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
    addAndMakeVisible (lpfLabel.get());
    lpfLabel->attachToComponent (lpfSlider.get(), false);
    lpfLabel->setJustificationType (juce::Justification::centred);
    
    lookAndFeelButton = std::make_unique<juce::TextButton>("LookAndFeel");
    addAndMakeVisible (lookAndFeelButton.get());
    lookAndFeelButton->addListener (this);
    
    theLFDark.setColourScheme (juce::LookAndFeel_V4::getDarkColourScheme());
    theLFMid.setColourScheme (juce::LookAndFeel_V4::getMidnightColourScheme());
    theLFGrey.setColourScheme (juce::LookAndFeel_V4::getGreyColourScheme());
    theLFLight.setColourScheme (juce::LookAndFeel_V4::getLightColourScheme());
    
    juce::LookAndFeel::setDefaultLookAndFeel (&theLFDark);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel (nullptr);
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto textBounds = bounds.removeFromTop (40);
    
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setColour (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillRect(bounds);
    
    
    //background
    g.setColour(juce::Colours::black);
    g.fillRect (bounds);
    
    //text
    g.setColour (juce::Colours::white);
    g.setFont (juce::Font (20.0f).italicised().withExtraKerningFactor (0.1f));
    g.drawFittedText("DSP Lesson 1", textBounds, juce::Justification::centredLeft, 1);

//    g.setColour (juce::Colours::white);
//    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto bounds = getLocalBounds();
    auto rectTop = bounds.removeFromTop (40);
    bounds.reduce (40, 40);
    
    rectTop.reduce (10, 0);
    lookAndFeelButton->setBounds(rectTop.removeFromRight (120).withSizeKeepingCentre (120, 24));
    
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

void NewProjectAudioProcessorEditor::buttonClicked(juce::Button* button)
{
        if (button == lookAndFeelButton.get())
        {
            juce::PopupMenu m;
            
            m.addItem (1, "Dark look and feel", true, currentLF == 1);
            m.addItem (2, "Midnight look and feel", true, currentLF == 2);
            m.addItem (3, "Grey look and feel", true, currentLF == 3);
            m.addItem (4, "Light look and feel", true, currentLF == 4);
            
            m.addSeparator();
            m.addItem (5, "JUCE 4 look and feel", true, currentLF == 5);
            m.addItem (6, "JUCE 3 look and feel", true, currentLF == 6);
            
            auto result = m.showAt(lookAndFeelButton.get());
            
            if (result == 1)
                juce::LookAndFeel::setDefaultLookAndFeel (&theLFDark);
            
            else if (result == 2)
                juce::LookAndFeel::setDefaultLookAndFeel (&theLFMid);
            
            else if (result == 3)
                juce::LookAndFeel::setDefaultLookAndFeel (&theLFGrey);
            
            else if (result == 4)
                juce::LookAndFeel::setDefaultLookAndFeel(&theLFLight);
            
            else if (result == 5)
                juce::LookAndFeel::setDefaultLookAndFeel (&theLFV3);
            
            else if (result == 6)
                juce::LookAndFeel::setDefaultLookAndFeel (&theLFV2);
            
            if (result != 0)
                currentLF = result;
        }
}
