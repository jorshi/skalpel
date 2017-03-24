/*
  ==============================================================================

    SoundInterfaceManager.cpp
    Created: 23 Mar 2017 10:26:01pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SoundInterfaceManager.h"


SoundInterfaceManager::SoundInterfaceManager() : Thread("SoundInterface Manager Thread")
{
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

