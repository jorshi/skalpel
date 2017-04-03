/*
  ==============================================================================

    VisualizerComponent.h
    Created: 2 Apr 2017 10:17:06pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef VISUALIZERCOMPONENT_H_INCLUDED
#define VISUALIZERCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SoundInterface.h"

//==============================================================================
/*
*/
class VisualizerComponent : public Component, private Timer
{
public:
    VisualizerComponent(SoundInterface* s);
    ~VisualizerComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void run();
    void stop();
    
    void drawNextLine();

private:
    
    void timerCallback() override;

    SoundInterface* sound;
    
    Image spectrogramImage;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualizerComponent)
};


#endif  // VISUALIZERCOMPONENT_H_INCLUDED
