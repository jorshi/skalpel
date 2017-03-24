/*
  ==============================================================================

    SoundInterfaceManager.h
    Created: 23 Mar 2017 10:26:01pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SOUNDINTERFACEMANAGER_H_INCLUDED
#define SOUNDINTERFACEMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SoundInterface.h"
#include "AnalysisParameterManager.h"


class SoundInterfaceManager : private Thread
{
public:
    
    // Constructor
    SoundInterfaceManager(int numSounds, AudioProcessorValueTreeState* params);
    
    // Default Deconstructor
    ~SoundInterfaceManager();
    
    // Get Sound Interface
    SoundInterface* getInterface(int num) { return soundInterfaces_[num]; }
    
    // Get an array of pointers to active sounds
    Array<SoundInterface*> getActiveSounds();

private:
    
    // Run the thread
    void run() override;
    
    // Sound interfaces and analysis parameters being managed here
    OwnedArray<SoundInterface> soundInterfaces_;
    OwnedArray<AnalysisParameterManager> analysisParameters_;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundInterfaceManager)
};





#endif  // SOUNDINTERFACEMANAGER_H_INCLUDED
