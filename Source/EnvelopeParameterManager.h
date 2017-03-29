/*
  ==============================================================================

    EnvelopeParameterManager.h
    Created: 27 Mar 2017 11:04:17am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ENVELOPEPARAMETERMANAGER_H_INCLUDED
#define ENVELOPEPARAMETERMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterManager.h"
#include "ModulationFactory.h"
#include "Modulation.h"
#include "ADSR.h"

class EnvelopeParamaterManager : public ParameterManager
{
public:
    EnvelopeParamaterManager(int num, AudioProcessorValueTreeState* p, ModulationFactory* m);
    
    ~EnvelopeParamaterManager() override {};
    
    void createParameters() override;
    
    bool getRawValue(const String& name, float& value) const;
    
private:
    
    Array<String> availableEnvelopes_;
    ModulationFactory* modulationFactory_;
};



#endif  // ENVELOPEPARAMETERMANAGER_H_INCLUDED
