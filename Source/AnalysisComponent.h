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
    
    TextButton analysisButton;
    
    AnalysisParameterManager* analysisParams;
    
    // Parameter Components
    std::map<String, ComboBox*> comboBoxMap;
    OwnedArray<ComboBox> comboBoxes;
    
    ComboBox windowSize;
    
    // Combo Box Attacher
    ScopedPointer<ComboBoxAttachment> comboAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisComponent)
};


#endif  // ANALYSISCOMPONENT_H_INCLUDED
