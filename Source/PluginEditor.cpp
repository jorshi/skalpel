/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//TODO: convert this button listener with broadcasts from children to use the new action listener

//==============================================================================
LoomAudioProcessorEditor::LoomAudioProcessorEditor (LoomAudioProcessor& p) :
    AudioProcessorEditor (&p), processor (p), soundInterface(p.getCurrentSound()),
    analysisComponent(this, soundInterface->getAnalysisParams()),
    loadComponent(this, this, soundInterface), synthesisComponent(this)
{
    setLookAndFeel(&loomLookAndFeel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 360);
    
    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    gradientMain = ColourGradient(Colour::fromRGB(63, 63, 63), 0.0f, 61.0f, Colour::fromRGB(43, 43, 43), 0.0f, 278.0f, false);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 640, 360);
    header = Rectangle<int>(0, 0, 640, 51);
    footer = Rectangle<int>(0, 288, 640, 74);
    middle = Rectangle<int>(10, 61, 620, 217);
    
    // Components
    addAndMakeVisible(analysisComponent);
    addAndMakeVisible(loadComponent);
    addAndMakeVisible(synthesisComponent);
    
    loadState();
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
    
    g.setColour(LoomLookAndFeel::loomTextColour);
    Font loomFont = Font(LoomLookAndFeel::daysOneTypeFace);
    loomFont.setHeight(51);
    g.setFont(loomFont);
    g.drawText("LOOM", 25, 0, 182, 51, Justification::left);
    
}

void LoomAudioProcessorEditor::resized()
{
    analysisComponent.setBounds(10, 61, 620, 217);
    loadComponent.setBounds(10, 61, 620, 217);
    synthesisComponent.setBounds(10, 61, 620, 217);
}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    
    if (button->getComponentID() == "load_file")
    {
        soundInterface->loadFile();
        loadState();
    }
    
    if (button->getComponentID() == "run_analysis")
    {
        soundInterface->runAnalysis();
        //processor.swapSound(soundInterface);
        switchState(SoundInterface::synthesisState);
    }
    
    if (button->getComponentID() == "new_analysis")
    {
        switchState(SoundInterface::loadFileState);
    }
}


void LoomAudioProcessorEditor::switchState(SoundInterface::State newState)
{
    if (soundInterface->getState() != newState)
    {
        soundInterface->setState(newState);
        loadState();
    }
}


void LoomAudioProcessorEditor::hideMiddle()
{
    analysisComponent.setVisible(false);
    loadComponent.setVisible(false);
    synthesisComponent.setVisible(false);
}


void LoomAudioProcessorEditor::loadState()
{
    hideMiddle();
    switch(soundInterface->getState())
    {
        case SoundInterface::loadFileState:
            loadComponent.setVisible(true);
            break;
            
        case SoundInterface::analysisState:
            analysisComponent.setVisible(true);
            break;
            
        case SoundInterface::runningAnalysisState:
            break;
            
        case SoundInterface::synthesisState:
            synthesisComponent.setVisible(true);
            break;
    }
}


void LoomAudioProcessorEditor::actionListenerCallback(const String& message)
{
    if (message == "reload_state")
    {
        loadState();
    }
}


