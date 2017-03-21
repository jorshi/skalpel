/*
  ==============================================================================

    LoadComponent.cpp
    Created: 20 Mar 2017 10:18:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoadComponent.h"

//==============================================================================
LoadComponent::LoadComponent(ButtonListener* parent)
{
    setLookAndFeel(&loomLookAndFeel);
    
    openButton.setButtonText("Load File");
    openButton.setComponentID("load_file");
    openButton.addListener(parent);
    addAndMakeVisible(&openButton);
}

LoadComponent::~LoadComponent()
{
}

void LoadComponent::paint (Graphics& g)
{
}

void LoadComponent::resized()
{
    openButton.setBounds(241, 92, 138, 34);
}
