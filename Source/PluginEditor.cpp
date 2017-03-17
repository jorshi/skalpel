/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LoomAudioProcessorEditor::LoomAudioProcessorEditor (LoomAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 360);
    
    openButton.setButtonText("Open");
    openButton.addListener(this);
    addAndMakeVisible(&openButton);
    
    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 640, 360);
    header = Rectangle<int>(0, 0, 640, 51);
    footer = Rectangle<int>(0, 288, 640, 69);
}

LoomAudioProcessorEditor::~LoomAudioProcessorEditor()
{
}

//==============================================================================
void LoomAudioProcessorEditor::paint (Graphics& g)
{
    // Draw rectangles
    g.setColour(bgColour);
    g.fillRect(background);
    
    g.setColour(layer1Colour);
    g.fillRect(header);
    g.fillRect(footer);
}

void LoomAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    openButton.setBounds(0, 0, 100, 25);
}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    processor.newAnalysis();
}


