/*
  ==============================================================================

    ModulationComponent.h
    Created: 2 Apr 2017 1:55:44am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MODULATIONCOMPONENT_H_INCLUDED
#define MODULATIONCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomLookAndFeel.h"
#include "EnvelopeParameterManager.h"

//==============================================================================
/*
*/
class ModulationComponent    : public Component, public ActionListener
{
public:
    ModulationComponent(ActionListener* parent, EnvelopeParamaterManager* p);
    ~ModulationComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    
    // Get called when a child sends a message: allows messages to bubble up
    void actionListenerCallback(const String& message) override;
    
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    
    EnvelopeParamaterManager* params;
    
    // Sliders
    Slider attack;
    Slider decay;
    Slider sustain;
    Slider release;
    
    // Labels
    Label attackLabel;
    Label decayLabel;
    Label sustainLabel;
    Label releaseLabel;
    
    // Attachments
    ScopedPointer<SliderAttachment> attackAttachment;
    ScopedPointer<SliderAttachment> decayAttachment;
    ScopedPointer<SliderAttachment> sustainAttachment;
    ScopedPointer<SliderAttachment> releaseAttachment;
    
    ActionBroadcaster broadcaster;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationComponent)
};


#endif  // MODULATIONCOMPONENT_H_INCLUDED
