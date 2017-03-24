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
    
    for (int i = 0; i < numSounds; i++)
    {
        // New set of analysis parameters
        analysisParams = new AnalysisParameterManager(i, params);
        analysisParameters_.insert(i, analysisParams);
       
        // New sound interface for this particular sound
        sound = new SoundInterface(analysisParams);
        soundInterfaces_.insert(i, sound);
    }
    
    startThread();
}


SoundInterfaceManager::~SoundInterfaceManager()
{
    stopThread(4000);
}


void SoundInterfaceManager::run()
{
    while(!threadShouldExit())
    {
        // Check to see if there are resources that can be cleaned up, also,
        // perhaps the analysis can be run on this thread!
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

