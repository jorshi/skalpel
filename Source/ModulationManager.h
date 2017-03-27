/*
  ==============================================================================

    ModulationManager.h
    Created: 27 Mar 2017 2:36:57am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MODULATIONMANAGER_H_INCLUDED
#define MODULATIONMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulation.h"
#include "ModulationFactory.h"
#include <map>

class ModulationManager
{
public:
    
    // Constructor
    ModulationManager() {};
    
    // Deconstructor
    ~ModulationManager() {};
    
    // Apply modulation from a set of modulation sources to a value, and then
    // increment each of those modulation sources by a number of samples, default 1
    void apply(float& value, String id, int sampleInc=1);
    
    // Add a new modulation source link to a parameter name
    int addModulation(String paramName, String sourceId);
    
private:
    
    typedef std::map<String, ReferenceCountedArray<Modulation>> ModMatrix;
    
    // Mapping of modulatable destination parameters, referenced by an idea, to an array of modulation
    // sources that will be applied to that source
    ModMatrix modMatrix_;
};



#endif  // MODULATIONMANAGER_H_INCLUDED
