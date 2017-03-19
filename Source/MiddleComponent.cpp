/*
  ==============================================================================

    MiddleComponent.cpp
    Created: 19 Mar 2017 1:09:01am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MiddleComponent.h"

//==============================================================================
MiddleComponent::MiddleComponent(ButtonListener* parent, SoundInterface& s) :
    soundInterface(s), analysisComponent(s.getAnalysisParams())
{
    setLookAndFeel(&loomLookAndFeel);
    
    gradientMain = ColourGradient(Colour::fromRGB(63, 63, 63), 0.0f, 61.0f, Colour::fromRGB(43, 43, 43), 0.0f, 278.0f, false);
    
    // Setup buttons
    openButton.setButtonText("Load File");
    openButton.setComponentID("load_file");
    openButton.addListener(parent);
    addAndMakeVisible(&openButton);
    
    analysisButton.setButtonText("Run Analysis");
    analysisButton.setComponentID("run_analysis");
    analysisButton.addListener(parent);
    addAndMakeVisible(&analysisButton);
    
    newButton.setButtonText("New Analysis");
    newButton.setComponentID("new_analysis");
    newButton.addListener(parent);
    addAndMakeVisible(&newButton);
    
    state = soundInterface.getState();
    
    // child components
    addAndMakeVisible(analysisComponent);
}

MiddleComponent::~MiddleComponent()
{
}

void MiddleComponent::paint (Graphics& g)
{
    g.setGradientFill(gradientMain);
    g.fillAll();
    
    g.setColour(Colours::black);
    g.drawRect(getLocalBounds(), 1);
}

void MiddleComponent::resized()
{
    openButton.setBounds(241, 92, 138, 34);
    analysisButton.setBounds(241, 166, 138, 34);
    newButton.setBounds(241, 166, 138, 34);
    
    analysisComponent.setBounds(getLocalBounds());
}


void MiddleComponent::updateState()
{
    state = soundInterface.getState();
    loadState();
}


void MiddleComponent::hideAllButtons()
{
    openButton.setVisible(false);
    newButton.setVisible(false);
    analysisButton.setVisible(false);
}


void MiddleComponent::loadState()
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

