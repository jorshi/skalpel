/*
  ==============================================================================

    LoadComponent.h
    Created: 20 Mar 2017 10:18:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef LOADCOMPONENT_H_INCLUDED
#define LOADCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomLookAndFeel.h"
#include "LoomFileDrop.h"
#include "SoundInterface.h"

//==============================================================================
/*
*/
class LoadComponent    : public Component, public ActionListener
{
public:
    LoadComponent(ButtonListener* parent, ActionListener* parentAction, SoundInterface* s);
    ~LoadComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    // Get called when a child sends a message: allows messages to bubble up
    void actionListenerCallback(const String& message) override;
    
    LoomLookAndFeel loomLookAndFeel;
    TextButton openButton;

    
    // Sound interface for this loading component
    SoundInterface* soundInterface;
    
    // File drop component
    LoomFileDrop loomFileDrop;
    
    // Message broadcaster
    ActionBroadcaster broadcaster;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoadComponent)
};


#endif  // LOADCOMPONENT_H_INCLUDED
