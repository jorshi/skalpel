/*
  ==============================================================================

    ParameterManager.cpp
    Created: 18 Mar 2017 8:49:41pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "ParameterManager.h"

// Default Constructor
ParameterManager::ParameterManager()
{
}


// Defualt Deconstructor
ParameterManager::~ParameterManager()
{
}

AudioParameterFloat* ParameterManager::createParameter(juce::String parameterID, juce::String parameterName, NormalisableRange<float> range, float defaultValue, bool managed)
{
    AudioParameterFloat* newParamater = new AudioParameterFloat(parameterID, parameterName, range, defaultValue);
    
    // Save parameter in map
    
    parameterMap_.emplace(parameterID, newParamater);
    
    // If we are responsible for managing this object
    if (managed)
    {
        managedParameters_.add(newParamater);
    }
    
    return newParamater;
}
