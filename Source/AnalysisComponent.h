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

//==============================================================================
/*
*/
class AnalysisComponent    : public Component
{
public:
    AnalysisComponent();
    ~AnalysisComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisComponent)
};


#endif  // ANALYSISCOMPONENT_H_INCLUDED
