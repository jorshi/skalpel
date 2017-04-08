/*
  ==============================================================================

    SoundInterfaceManager.cpp
    Created: 23 Mar 2017 10:26:01pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SoundInterfaceManager.h"


SoundInterfaceManager::SoundInterfaceManager(int numSounds, AudioProcessorValueTreeState* params) :
Thread("SoundInterface Manager Thread")
{
    SoundInterface* sound;
    AnalysisParameterManager* analysisParams;
    SynthesisParameterManager* synthParams;
    
    for (int i = 0; i < numSounds; i++)
    {
        // New set of analysis parameters
        analysisParams = new AnalysisParameterManager(i, params);
        analysisParameters_.insert(i, analysisParams);
        
        // New set of synthesis parameters
        synthParams = new SynthesisParameterManager(i, params);
        synthesisParameters_.insert(i, synthParams);
       
        // New sound interface for this particular sound
        sound = new SoundInterface(analysisParams, synthParams);
        soundInterfaces_.insert(i, sound);
    }
    
    currentUISound = 0;
 
    modulationFactory_ = new ModulationFactory();
    envelopes_ = new EnvelopeParamaterManager(0, params, modulationFactory_);
    
    startThread();
}


SoundInterfaceManager::~SoundInterfaceManager()
{
    stopThread(4000);
}


SoundInterface* SoundInterfaceManager::operator[](const int num) const
{
    return soundInterfaces_.getUnchecked(num);
}


void SoundInterfaceManager::run()
{
    while(!threadShouldExit())
    {
        // Check to see if there are resources that can be cleaned up, also,
        // perhaps the analysis can be run on this thread!
        for (int i = 0; i < soundInterfaces_.size(); i++)
        {
            soundInterfaces_.getUnchecked(i)->checkModels();
        }
        wait(500);
    }
}


Array<SoundInterface*> SoundInterfaceManager::getActiveSounds()
{
    Array<SoundInterface*> activeSounds;
    for (int i = 0; i < soundInterfaces_.size(); i++)
    {
        if (soundInterfaces_[i]->isActive())
        {
            activeSounds.add(soundInterfaces_[i]);
        }
    }
    
    return activeSounds;
}


Modulation::Ptr SoundInterfaceManager::getModulator(const String& id) const
{
    return modulationFactory_->make(id);
}

