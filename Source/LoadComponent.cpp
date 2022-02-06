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
LoadComponent::LoadComponent(Button::Listener* parent, ActionListener* parentAction, SoundInterface* s) :
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
    g.setColour(LoomLookAndFeel::loomTextColour);
    g.drawText("Load Audio For Analysis", 0, 10, getWidth(), 15, Justification::centred);
}

void LoadComponent::resized()
{
    openButton.setBounds((getWidth()/2.0f) - (330/2.0f), 205, 330, 34);
    loomFileDrop.setBounds((getWidth()/2.0f) - (330/2.0f), 60, 330, 128);
}

void LoadComponent::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}
