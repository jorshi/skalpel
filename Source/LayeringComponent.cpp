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
LayeringComponent::LayeringComponent(ActionListener* parent, SoundInterfaceManager* s) : soundManager(s)
{
    // Setup action broadcaster
    broadcaster.removeAllActionListeners();
    broadcaster.addActionListener(parent);
    
    loomLookAndFeel.setButtonRadius(1.0f);
    setLookAndFeel(&loomLookAndFeel);

    TextButton* newButton;
    for (int i = 0; i < soundManager->size(); i++)
    {
        newButton = new TextButton;
        newButton->setButtonText("Layer " + std::to_string(i+1));
        newButton->setClickingTogglesState(true);
        newButton->setRadioGroupId(1);
        newButton->addListener(this);
        layerButtons.add(newButton);
        addAndMakeVisible(newButton);
    }
    
    layerButtons.getUnchecked(soundManager->getCurrentSound())->setToggleState(true, sendNotification);
}


LayeringComponent::~LayeringComponent()
{
}


void LayeringComponent::paint (Graphics& g)
{
}


void LayeringComponent::resized()
{
    float buttonWidth = getWidth() / layerButtons.size();
    for (int i = 0; i < layerButtons.size(); i++)
    {
        layerButtons.getUnchecked(i)->setBounds((buttonWidth * i) + 3, 3, buttonWidth - 5, getHeight() - 5);
    }
}


void LayeringComponent::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}


void LayeringComponent::buttonClicked(Button* button)
{
    for (int i = 0; i < layerButtons.size(); i++)
    {
        if (button == layerButtons.getUnchecked(i) && layerButtons.getUnchecked(i)->getToggleState())
        {
            soundManager->setCurrentSound(i);
            broadcaster.sendActionMessage("layer_change");
        }
    }
}
