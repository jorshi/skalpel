/*
  ==============================================================================

    ModulationFactory.h
    Created: 27 Mar 2017 12:56:52am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MODULATIONFACTORY_H_INCLUDED
#define MODULATIONFACTORY_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include "Modulation.h"
#include "ADSR.h"


class ModulationFactory
{
public:
    
    // Default Constructor
    ModulationFactory();
    
    // Clone a prototype modulation source and return a pointer to it
    static Modulation::Ptr make(String id);
    
    static bool addModulationPrototype(Modulation::Ptr modSource, String id);
    
private:
    typedef std::map<String, Modulation::Ptr> ModMap;

    // Name set of modulation sources
    static ModMap modulationPrototypes_;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationFactory)
};

#endif  // MODULATIONFACTORY_H_INCLUDED
