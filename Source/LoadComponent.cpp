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
LoadComponent::LoadComponent(ButtonListener* parent, ActionListener* parentAction, SoundInterface& s) :
    soundInterface(s), loomFileDrop(this, s)
{
    setLookAndFeel(&loomLookAndFeel);
    
    openButton.setButtonText("Browse");
    openButton.setComponentID("load_file");
    openButton.addListener(parent);
    addAndMakeVisible(&openButton);
    
    addAndMakeVisible(&loomFileDrop);
    
    // Setup action broadcaster
    broadcaster.removeAllActionListeners();
    broadcaster.addActionListener(parentAction);
}

LoadComponent::~LoadComponent()
{
}

void LoadComponent::paint (Graphics& g)
{
}

void LoadComponent::resized()
{
    openButton.setBounds(145, 165, 330, 34);
    loomFileDrop.setBounds(145, 20, 330, 128);
}

void LoadComponent::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}
