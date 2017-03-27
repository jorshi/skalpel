/*
  ==============================================================================

    ModulationManager.cpp
    Created: 27 Mar 2017 2:36:57am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "ModulationManager.h"

void ModulationManager::apply(float &value, String id, int sampleInc)
{
    ModMatrix::iterator modSources = modMatrix_.find(id);
    if (modSources != modMatrix_.end())
    {
        for (int i = 0; i < modSources->second.size(); i++)
        {
            modSources->second.getUnchecked(i)->apply(value, sampleInc);
        }
    }
}


int ModulationManager::addModulation(String paramName, String sourceId)
{
    // Get a clone of a modulation object from the modulation factory
    ModMatrix::iterator modSources = modMatrix_.find(paramName);
    int modulationPostion = 0;
    if (modSources != modMatrix_.end())
    {
        modulationPostion = modSources->second.size();
        // Save that object at the end of the list
    }
    else
    {
        //modMatrix_.emplace(paramName);
    }
    
    return modulationPostion;
}
