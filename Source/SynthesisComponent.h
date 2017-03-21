/*
  ==============================================================================

    SynthesisComponent.h
    Created: 20 Mar 2017 10:38:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SYNTHESISCOMPONENT_H_INCLUDED
#define SYNTHESISCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomLookAndFeel.h"

//==============================================================================
/*
*/
class SynthesisComponent    : public Component
{
public:
    SynthesisComponent(ButtonListener* parent);
    ~SynthesisComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    LoomLookAndFeel loomLookAndFeel;
    TextButton newButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesisComponent)
};


#endif  // SYNTHESISCOMPONENT_H_INCLUDED
