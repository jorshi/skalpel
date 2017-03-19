/*
  ==============================================================================

    AnalysisComonent.h
    Created: 19 Mar 2017 12:33:35am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ANALYSISCOMONENT_H_INCLUDED
#define ANALYSISCOMONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class AnalysisComonent    : public Component
{
public:
    AnalysisComonent();
    ~AnalysisComonent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisComonent)
};


#endif  // ANALYSISCOMONENT_H_INCLUDED
