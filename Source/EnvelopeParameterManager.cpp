/*
  ==============================================================================

    EnvelopeParameterManager.cpp
    Created: 27 Mar 2017 11:04:17am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "EnvelopeParameterManager.h"


EnvelopeParamaterManager::EnvelopeParamaterManager(int num, AudioProcessorValueTreeState* p) :
    ParameterManager(num, p)
{
    createParameters();
}


void EnvelopeParamaterManager::createParameters()
{
    availableEnvelopes_.add("adsr_1");
    parameters_->createAndAddParameter("adsr_1_attack",
                                       "ADSR 1 Attack",
                                       "ms",
                                       NormalisableRange<float>(0.1f, 5000.0f, 10.0f, 0.75f),
                                       10.0f,
                                       nullptr,
                                       nullptr);
    
    ADSR* newAdsr = new ADSR(parameters_);
    newAdsr->registerAttackParam("adsr_1_attack");
    ModulationFactory::addModulationPrototype(newAdsr, "adsr_1");
}
