/*
  ==============================================================================

    SynthesisParameterManager.h
    Created: 30 Mar 2017 12:32:12am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SYNTHESISPARAMETERMANAGER_H_INCLUDED
#define SYNTHESISPARAMETERMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterManager.h"

class SynthesisParameterManager : public ParameterManager
{
public:

    // Constructor
    SynthesisParameterManager(int num, AudioProcessorValueTreeState* p);
    
    // Deconstructor
    ~SynthesisParameterManager() {};
    
    // Creates all the parameters for a particular sound
    void createParameters() override;
    
    bool getRawValue(const String& name, float& value) const;
    
private:
  

    JUCE_LEAK_DETECTOR(SynthesisParameterManager)
};



#endif  // SYNTHESISPARAMETERMANAGER_H_INCLUDED
