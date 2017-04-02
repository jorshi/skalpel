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

//==============================================================================
/*
*/
class LayeringComponent    : public Component, public ActionListener
{
public:
    LayeringComponent(ActionListener* parent);
    ~LayeringComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    // Get called when a child sends a message: allows messages to bubble up
    void actionListenerCallback(const String& message) override;
    
    // Message broadcaster
    ActionBroadcaster broadcaster;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayeringComponent)
};


#endif  // LAYERINGCOMPONENT_H_INCLUDED
