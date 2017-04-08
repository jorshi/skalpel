/*
  ==============================================================================

    ModulationFactory.cpp
    Created: 27 Mar 2017 12:56:52am
    Author:  Jordie Shier

  ==============================================================================
*/

#include "ModulationFactory.h"

ModulationFactory::ModulationFactory()
{
    modulationPrototypes_.emplace("null", new NullModulation());
}


Modulation::Ptr ModulationFactory::make(String id)
{
    ModMap::iterator source = modulationPrototypes_.find(id);
    if (source != modulationPrototypes_.end())
    {
        return source->second->clone();
    }
    else if ((source = modulationPrototypes_.find("null")) != modulationPrototypes_.end())
    {
        // Try to return a Null Modulator object, which is a modulator object that has no effect
        // on the signals passed into it
        return source->second->clone();
    }

    return nullptr;
}

bool ModulationFactory::addModulationPrototype(Modulation::Ptr modSource, String id)
{
    std::pair<ModMap::iterator, bool> result = modulationPrototypes_.emplace(id, modSource);
    return result.second;
}
