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
#include "SynthesisParameterManager.h"
#include "SoundInterface.h"
#include "VisualizerComponent.h"

//==============================================================================
/*
*/
class SynthesisComponent    : public Component
{
public:
    SynthesisComponent(ButtonListener* parent, SoundInterface* s);
    ~SynthesisComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void setVisible(bool shouldBeVisible) override;

private:
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    LoomLookAndFeel loomLookAndFeel;
    TextButton newButton;
    TextButton analysisButton;

    Rectangle<int> visualizer;

    ScopedPointer<VisualizerComponent> visualizerComponent;
    
    SoundInterface* sound;
    SynthesisParameterManager* synthesisParams;

    int bottomRowOffset;
    int groupA;
    int groupB;
    int groupC;
    int groupD;
    int groupE;

    // Sliders
    Slider octave;
    Slider semitone;
    Slider cent;
    Slider multiply;
    Slider shift;
    Slider warpCenter;
    Slider warpAmount;
    Slider timeOffset;
    Slider playbackRate;
    Slider sineCount;
    Slider sineGain;
    Slider noiseGain;

    // Labels
    Label octaveLabel;
    Label semitoneLabel;
    Label centLabel;
    Label multiplyLabel;
    Label shiftLabel;
    Label warpCenterLabel;
    Label warpAmountLabel;
    Label timeOffsetLabel;
    Label playbackRateLabel;
    Label sineCountLabel;
    Label sineGainLabel;
    Label noiseGainLabel;

    // Attachments
    ScopedPointer<SliderAttachment> octaveAttachment;
    ScopedPointer<SliderAttachment> semitoneAttachment;
    ScopedPointer<SliderAttachment> centAttachment;
    ScopedPointer<SliderAttachment> multiplyAttachment;
    ScopedPointer<SliderAttachment> shiftAttachment;
    ScopedPointer<SliderAttachment> warpCenterAttachment;
    ScopedPointer<SliderAttachment> warpAmountAttachment;
    ScopedPointer<SliderAttachment> timeOffsetAttachment;
    ScopedPointer<SliderAttachment> playbackRateAttachment;
    ScopedPointer<SliderAttachment> sineCountAttachment;
    ScopedPointer<SliderAttachment> sineGainAttachment;
    ScopedPointer<SliderAttachment> noiseGainAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesisComponent)
};


#endif  // SYNTHESISCOMPONENT_H_INCLUDED
