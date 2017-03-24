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

class SoundInterfaceManager : private Thread
{
public:
    
    // Default Construcotr
    SoundInterfaceManager();
    
    // Default Deconstructor
    ~SoundInterfaceManager();

private:
    
    // Run the thread
    void run() override;
    
    // SoundInterfaces being managed here
    OwnedArray<SoundInterface> soundInterfaces_;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundInterfaceManager)
};





#endif  // SOUNDINTERFACEMANAGER_H_INCLUDED
