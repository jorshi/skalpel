/*
  ==============================================================================

    AnalysisComponent.h
    Created: 19 Mar 2017 12:36:26am
    Author:  Jordie Shier

  ==============================================================================
*/

#ifndef ANALYSISCOMPONENT_H_INCLUDED
#define ANALYSISCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LoomLookAndFeel.h"
#include "ParameterManager.h"
#include "AnalysisParameterManager.h"
#include <map>

//==============================================================================
/*
*/
class AnalysisComponent :   public Component
{
public:
    AnalysisComponent(ButtonListener* parent, AnalysisParameterManager* p);
    ~AnalysisComponent();

    void paint (Graphics&) override;
    void resized() override;

    void addComboBox(String paramId);

    void addComboBoxOptions(ComboBox* box, const String paramId);

private:
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    TextButton analysisButton;

    AnalysisParameterManager* analysisParams;

    // Combo Boxes
    ComboBox windowSize;
    ComboBox hopSize;

    // Sliders
    Slider amplitudeTreshold;
    Slider duration;
    Slider freqOffset;
    Slider freqSlope;
    Slider sines;

    // Labels
    Label windowLabel;
    Label amplitudeLabel;
    Label durationLabel;
    Label freqOffsetLabel;
    Label sinesLabel;

    // Combo Box Attacher
    ScopedPointer<ComboBoxAttachment> windowAttachment;
    ScopedPointer<ComboBoxAttachment> hopAttachment;


    // Slider Attachments
    ScopedPointer<SliderAttachment> amplitudeAttachment;
    ScopedPointer<SliderAttachment> durationAttachment;
    ScopedPointer<SliderAttachment> freqOffsetAttachment;
    ScopedPointer<SliderAttachment> freqSlopeAttachment;
    ScopedPointer<SliderAttachment> sinesAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisComponent)
};


#endif  // ANALYSISCOMPONENT_H_INCLUDED
