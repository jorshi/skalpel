/*
  ==============================================================================

    SinusoidalSynthSound.cpp
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthSound.h"

// Empty model constructor
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch,
                                           int frameSize, SoundInterfaceManager* manager)
:   midiNotes_(notes),
    midiRootNote_(midiNoteForNormalPitch),
    frameSize_(frameSize),
    manager_(manager)
{

    // Blackman Harris window for spectral synthesis
    bh1001_.resize(1001);
    for (int i = 0; i < 1001; ++i)
    {
        bh1001_[i] = SynthUtils::BHCONST.at(i);
    }
    
    // Synthesis window for windowing time domain output of signal
    synthWindow_.resize(frameSize_);
    SynthUtils::createSynthesisWindow(synthWindow_, frameSize_/4);
};


// Destructor
SinusoidalSynthSound::~SinusoidalSynthSound()
{
};


// Sound for note?
bool SinusoidalSynthSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes_ [midiNoteNumber];
}


// Sound for midi channel?
bool SinusoidalSynthSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}


ReferenceCountedArray<SineModel> SinusoidalSynthSound::getPlayingSineModels() const
{
    ReferenceCountedArray<SineModel> models;
    Array<SoundInterface*> activeSounds = manager_->getActiveSounds();
    
    for (int i = 0; i < activeSounds.size(); i++)
    {
        models.add(activeSounds[i]->getSineModel());
    }

    return models;
}


Array<SoundInterface*> SinusoidalSynthSound::getPlayingSounds() const
{
    return manager_->getActiveSounds();
}


