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
#include "ModulationFactory.h"
#include "AnalysisParameterManager.h"
#include "EnvelopeParameterManager.h"
#include "SynthesisParameterManager.h"


class SoundInterfaceManager : private Thread
{
public:
    
    // Constructor
    SoundInterfaceManager(int numSounds, AudioProcessorValueTreeState* params);
    
    // Default Deconstructor
    ~SoundInterfaceManager();
    
    SoundInterface* operator[](const int num) const;
    
    // Get Sound Interface
    SoundInterface* getInterface(int num) { return soundInterfaces_[num]; }
    
    // Get an array of pointers to active sounds
    Array<SoundInterface*> getActiveSounds();
    
    // Get a reference to all the sound interfaces
    const OwnedArray<SoundInterface>& getSounds() const { return soundInterfaces_; };
    
    // Sound interface begin iterator
    SoundInterface** begin() { return soundInterfaces_.begin(); };
    
    // Sound interface end iterator
    SoundInterface** end() { return soundInterfaces_.end(); };
    
    // Modulator object
    Modulation::Ptr getModulator(const String& id) const;
    
    // Get modulation parameter manager
    EnvelopeParamaterManager* getEnvelopeParameterManager() const { return envelopes_; };
    
    // Number of sound interfaces being managed
    int size() const { return soundInterfaces_.size(); };
    
    int getCurrentSound() const { return currentUISound; };
    void setCurrentSound(const int num) { currentUISound = num; };

private:
    
    // Run the thread
    void run() override;
    
    // Current sound to display on UI
    int currentUISound;
    
    // Sound interfaces and analysis parameters being managed here
    OwnedArray<SoundInterface> soundInterfaces_;
    OwnedArray<AnalysisParameterManager> analysisParameters_;
    OwnedArray<SynthesisParameterManager> synthesisParameters_;
    ScopedPointer<EnvelopeParamaterManager> envelopes_;
    
    // Modulation Factory
    ScopedPointer<ModulationFactory> modulationFactory_;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundInterfaceManager)
};





#endif  // SOUNDINTERFACEMANAGER_H_INCLUDED
