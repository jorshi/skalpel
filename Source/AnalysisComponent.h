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
#include <map>

//==============================================================================
/*
*/
class AnalysisComponent :   public Component,
                            public ComboBoxListener
{
public:
    AnalysisComponent(ParameterManager& p);
    ~AnalysisComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    // Listener
    void comboBoxChanged(ComboBox* box) override;

private:
    
    ParameterManager& analysisParams;
    
    // Parameter Components
    std::map<String, ComboBox*> comboBoxMap;
    OwnedArray<ComboBox> comboBoxes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisComponent)
};


#endif  // ANALYSISCOMPONENT_H_INCLUDED
