/*
  ==============================================================================

    ParameterManager.h
    Created: 18 Mar 2017 8:49:41pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ParameterManager_H_INCLUDED
#define ParameterManager_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>

class ParameterManager
{
public:
    
    // Default Constructor
    ParameterManager();
    
    // Default Deconstructor
    ~ParameterManager();
    
    // Create a new audio float parameter
    AudioParameterFloat* createParameter(String parameterID, String parameterName,
                                         NormalisableRange<float>, float defaultValue, bool managed = true);
    
    float getValue(String parameterID) const { return *parameterMap_.at(parameterID); };
    
    
private:
    
    std::map<String, AudioParameterFloat*> parameterMap_;
    
    // Memory Managed Parameters
    OwnedArray<AudioParameterFloat> managedParameters_;
};



#endif  // ParameterManager_H_INCLUDED
