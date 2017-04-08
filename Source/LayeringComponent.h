/*
  ==============================================================================

    LayeringComponent.h
    Created: 2 Apr 2017 3:00:02am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef LAYERINGCOMPONENT_H_INCLUDED
#define LAYERINGCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomLookAndFeel.h"
#include "SoundInterfaceManager.h"

//==============================================================================
/*
*/
class LayeringComponent :   public Component,
                            public ActionListener,
                            public Button::Listener
{
public:
    LayeringComponent(ActionListener* parent, SoundInterfaceManager* s);
    ~LayeringComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    // Get called when a child sends a message: allows messages to bubble up
    void actionListenerCallback(const String& message) override;
    
    // Called when any button on UI is clicked
    void buttonClicked(Button* button) override;
    
    ActionBroadcaster broadcaster;
    OwnedArray<TextButton> layerButtons;
    SoundInterfaceManager* soundManager;
    
    LoomLookAndFeel loomLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayeringComponent)
};


#endif  // LAYERINGCOMPONENT_H_INCLUDED
