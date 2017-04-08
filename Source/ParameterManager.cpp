/*
  ==============================================================================

    ParameterManager.cpp
    Created: 18 Mar 2017 8:49:41pm
    Author:  Jordie Shier

  ==============================================================================
*/

#include "ParameterManager.h"

// Default Constructor
ParameterManager::ParameterManager(int num, AudioProcessorValueTreeState* p) :
    soundNum_(num), parameters_(p)
{
}


// Default Deconstructor
ParameterManager::~ParameterManager() {};


String ParameterManager::getParamId(const String paramName) const
{
    std::map<String, String>::const_iterator name = paramMap_.find(paramName);
    if (name != paramMap_.end())
    {
        return name->second;
    }

    return "";
}

