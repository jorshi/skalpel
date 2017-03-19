/*
  ==============================================================================

    MiddleComponent.h
    Created: 19 Mar 2017 1:09:01am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MIDDLECOMPONENT_H_INCLUDED
#define MIDDLECOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SoundInterface.h"
#include "LoomLookAndFeel.h"
#include "AnalysisComponent.h"

//==============================================================================
/*
*/
class MiddleComponent    : public Component
{
public:
    MiddleComponent(ButtonListener* parent, SoundInterface& s);
    ~MiddleComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void updateState();
    void hideAllButtons();
    void loadState();

private:
    
    LoomLookAndFeel loomLookAndFeel;
    SoundInterface& soundInterface;
    
    SoundInterface::State state;
    
    // Buttons
    TextButton openButton;
    TextButton analysisButton;
    TextButton newButton;
    
    // Colours
    ColourGradient gradientMain;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MiddleComponent)
};


#endif  // MIDDLECOMPONENT_H_INCLUDED
