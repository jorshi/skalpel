/*
  ==============================================================================

    EnvelopeParameterManager.cpp
    Created: 27 Mar 2017 11:04:17am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "EnvelopeParameterManager.h"


EnvelopeParamaterManager::EnvelopeParamaterManager(int num, AudioProcessorValueTreeState* p,
                                                   ModulationFactory* m) :
    ParameterManager(num, p), modulationFactory_(m)
{
    createParameters();
}


void EnvelopeParamaterManager::createParameters()
{
    availableEnvelopes_.add("adsr_1");
    parameters_->createAndAddParameter("adsr_1_attack",
                                       "ADSR 1 Attack",
                                       "ms",
                                       NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 0.75f),
                                       10.0f,
                                       nullptr,
                                       nullptr);
    
    parameters_->createAndAddParameter("adsr_1_decay",
                                       "ADSR 1 Decay",
                                       "ms",
                                       NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 0.75f),
                                       150.0f,
                                       nullptr,
                                       nullptr);
    
    parameters_->createAndAddParameter("adsr_1_sustain",
                                       "ADSR 1 Sustain",
                                       String(),
                                       NormalisableRange<float>(0.0f, 1.0f, 0.01f),
                                       1.0f,
                                       nullptr,
                                       nullptr);
    
    parameters_->createAndAddParameter("adsr_1_release",
                                       "ADSR 1 Release",
                                       "ms",
                                       NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 0.75f),
                                       1000.0f,
                                       nullptr,
                                       nullptr);
    
    ADSR* newAdsr = new ADSR(parameters_);
    newAdsr->registerAttackParam("adsr_1_attack");
    newAdsr->registerDecayParam("adsr_1_decay");
    newAdsr->registerSustainParam("adsr_1_sustain");
    newAdsr->registerReleaseParam("adsr_1_release");
    modulationFactory_->addModulationPrototype(newAdsr, "adsr_1");
}
