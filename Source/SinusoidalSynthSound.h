/*
  ==============================================================================

    SinusoidalSynthSound.h
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SINUSOIDALSYNTHSOUND_H_INCLUDED
#define SINUSOIDALSYNTHSOUND_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SineElement.h"
#include "SynthesisUtils.h"
#include "SoundInterface.h"
#include "SoundInterfaceManager.h"


class SinusoidalSynthSound : public SynthesiserSound
{
public:
    
    // Constructor
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch,
                         int frameSize, SoundInterfaceManager* manager);
    
    ~SinusoidalSynthSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    const int& getFrameSize() const { return frameSize_; };
    
    // Get a reference to all playing sinusoid models
    ReferenceCountedArray<SineModel> getPlayingSineModels() const;
    
    Array<SoundInterface*> getPlayingSounds() const;
    
    // Sample the blackman harris window
    double getBH(int index) const { return bh1001_[index]; };
    
    // Sample the synthesis window function
    double getSynthWindow(int index) const { return synthWindow_[index]; };
    
    // Get sound interface manager
    SoundInterfaceManager* getSoundInterfaceManager() const { return manager_; };
    
    // Reference to all sound interfaces
    const OwnedArray<SoundInterface>& getAllSounds() const { return manager_->getSounds(); };
    
private:
    friend class SinusoidalSynthVoice;
    
    BigInteger midiNotes_;
    int midiRootNote_;
    int frameSize_;
    std::vector<double> bh1001_;
    std::vector<double> synthWindow_;
    SoundInterfaceManager* manager_;
    
    JUCE_LEAK_DETECTOR(SinusoidalSynthSound)
};

#endif  // SINUSOIDALSYNTHSOUND_H_INCLUDED
