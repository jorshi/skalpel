/*
  ==============================================================================

    SynthesisComponent.cpp
    Created: 20 Mar 2017 10:38:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthesisComponent.h"

//==============================================================================
SynthesisComponent::SynthesisComponent(ButtonListener* parent)
{
    setLookAndFeel(&loomLookAndFeel);
    
    newButton.setButtonText("New");
    newButton.setComponentID("new_analysis");
    newButton.addListener(parent);
    addAndMakeVisible(&newButton);
}

SynthesisComponent::~SynthesisComponent()
{
}

void SynthesisComponent::paint (Graphics& g)
{
}

void SynthesisComponent::resized()
{
    newButton.setBounds(555, 11, 50, 27);
}
