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
    : AudioProcessorEditor (&p), processor (p), soundInterface(p.getCurrentSound()), middleComponent(this, soundInterface)
{
    
    setLookAndFeel(&loomLookAndFeel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 360);
    
    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 640, 360);
    header = Rectangle<int>(0, 0, 640, 51);
    footer = Rectangle<int>(0, 288, 640, 74);
    
    // Components
    addAndMakeVisible(middleComponent);
    middleComponent.updateState();
}

LoomAudioProcessorEditor::~LoomAudioProcessorEditor()
{
}

//==============================================================================
void LoomAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour(bgColour);
    g.fillRect(background);

    g.setColour(layer1Colour);
    g.fillRect(header);
    g.fillRect(footer);
    
    g.setColour(Colours::black);
    g.drawHorizontalLine(header.getBottom(), header.getWidth() - header.getRight(), header.getRight());
    g.drawHorizontalLine(footer.getBottom() - footer.getHeight(), footer.getWidth() - footer.getRight(), footer.getRight());
    
}

void LoomAudioProcessorEditor::resized()
{
    middleComponent.setBounds(10, 61, 620, 217);
}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    
    if (button->getComponentID() == "load_file")
    {
        ScopedPointer<File> file;
        if ((file = fileLoader.getNewAudioFile()) != nullptr)
        {
            soundInterface.buildAnalysis(*file);
            switchState(SoundInterface::analysisState);
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Audio Read Error", "Could not read input audio");
        }
    }
    
    if (button->getComponentID() == "run_analysis")
    {
        soundInterface.runAnalysis();
        processor.swapSound(soundInterface);
        switchState(SoundInterface::synthesisState);
    }
    
    if (button->getComponentID() == "new_analysis")
    {
        switchState(SoundInterface::loadFileState);
    }
}


void LoomAudioProcessorEditor::switchState(SoundInterface::State newState)
{
    if (soundInterface.getState() != newState)
    {
        soundInterface.setState(newState);
        middleComponent.updateState();
    }
}


