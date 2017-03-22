/*
  ==============================================================================

    LoomFileDrop.cpp
    Created: 21 Mar 2017 11:04:08pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomFileDrop.h"

//==============================================================================
LoomFileDrop::LoomFileDrop(ActionListener* parent, SoundInterface& s) :
    buttonEdgeColour(Colour::fromRGB(124, 141, 165)), soundInterface(s)
{
    broadcaster.removeAllActionListeners();
    broadcaster.addActionListener(parent);
}

LoomFileDrop::~LoomFileDrop()
{
}

void LoomFileDrop::paint (Graphics& g)
{
    Rectangle<float> buttonArea(0, 0, getWidth(), getHeight());
    g.setColour(buttonEdgeColour);
    g.drawRoundedRectangle(buttonArea, 7.0f, 1.5f);
}

void LoomFileDrop::resized()
{
}

bool LoomFileDrop::isInterestedInFileDrag(const juce::StringArray &files)
{
    return true;
}

void LoomFileDrop::filesDropped(const juce::StringArray &files, int x, int y)
{
    soundInterface.loadFile(files[0]);
    broadcaster.sendActionMessage("reload_state");
}

void LoomFileDrop::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}
