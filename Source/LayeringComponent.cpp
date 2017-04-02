/*
  ==============================================================================

    LayeringComponent.cpp
    Created: 2 Apr 2017 3:00:02am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LayeringComponent.h"

//==============================================================================
LayeringComponent::LayeringComponent(ActionListener* parent)
{
    // Setup action broadcaster
    broadcaster.removeAllActionListeners();
    broadcaster.addActionListener(parent);
}


LayeringComponent::~LayeringComponent()
{
}


void LayeringComponent::paint (Graphics& g)
{
}


void LayeringComponent::resized()
{
}


void LayeringComponent::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}
