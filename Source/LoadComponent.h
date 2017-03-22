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

//==============================================================================
/*
*/
class LoadComponent    : public Component
{
public:
    LoadComponent(ButtonListener* parent);
    ~LoadComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    LoomLookAndFeel loomLookAndFeel;
    TextButton openButton;
    LoomFileDrop loomFileDrop;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoadComponent)
};


#endif  // LOADCOMPONENT_H_INCLUDED
