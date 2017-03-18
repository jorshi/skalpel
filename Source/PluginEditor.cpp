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
    : AudioProcessorEditor (&p), processor (p), soundInterface(p.getCurrentSound())
{
    
    setLookAndFeel(&loomLookAndFeel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 360);
    
    // Setup buttons
    openButton.setButtonText("Load File");
    openButton.addListener(this);
    addAndMakeVisible(&openButton);
    
    analysisButton.setButtonText("Run Analysis");
    analysisButton.addListener(this);
    addAndMakeVisible(&analysisButton);
    
    newButton.setButtonText("New Analysis");
    newButton.addListener(this);
    addAndMakeVisible(&newButton);
    
    // Load current state (defined in sound interface)
    loadState();
    
    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    gradientMain = ColourGradient(Colour::fromRGB(63, 63, 63), 0.0f, 61.0f, Colour::fromRGB(43, 43, 43), 0.0f, 278.0f, false);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 640, 360);
    header = Rectangle<int>(0, 0, 640, 51);
    footer = Rectangle<int>(0, 288, 640, 74);
    middle = Rectangle<int>(10, 61, 620, 217);
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
    
    g.setGradientFill(gradientMain);
    g.fillRect(middle);
    
    g.setColour(Colours::black);
    g.drawRect(middle);
    g.drawHorizontalLine(header.getBottom(), header.getWidth() - header.getRight(), header.getRight());
    g.drawHorizontalLine(footer.getBottom() - footer.getHeight(), footer.getWidth() - footer.getRight(), footer.getRight());
    
    // State dependent painting
    switch (soundInterface.getState()) {
        case SoundInterface::runningAnalysisState:
            g.drawText("Running Analysis", 0, 0, 50, 10, Justification::centred);
            break;
            
        default:
            break;
    }
}

void LoomAudioProcessorEditor::resized()
{
    openButton.setBounds(251, 163, 138, 34);
    analysisButton.setBounds(251, 227, 138, 34);
    newButton.setBounds(251, 227, 138, 34);

}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    
    if (button == &openButton)
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
    
    if (button == &analysisButton)
    {
        //switchState(SoundInterface::runningAnalysisState);
        soundInterface.runAnalysis();
        processor.swapSound(soundInterface);
        switchState(SoundInterface::synthesisState);
    }
    
    if (button == &newButton)
    {
        switchState(SoundInterface::loadFileState);
    }
}


void LoomAudioProcessorEditor::switchState(SoundInterface::State newState)
{
    if (soundInterface.getState() != newState)
    {
        soundInterface.setState(newState);
        loadState();
    }
}


void LoomAudioProcessorEditor::hideAllButtons()
{
    openButton.setVisible(false);
    newButton.setVisible(false);
    analysisButton.setVisible(false);
}


void LoomAudioProcessorEditor::loadState()
{
    hideAllButtons();
    switch(soundInterface.getState())
    {
        case SoundInterface::loadFileState:
            openButton.setVisible(true);
            break;
            
        case SoundInterface::analysisState:
            analysisButton.setVisible(true);
            break;
            
        case SoundInterface::runningAnalysisState:
            repaint();
            break;
            
        case SoundInterface::synthesisState:
            newButton.setVisible(true);
            break;
    }
}


