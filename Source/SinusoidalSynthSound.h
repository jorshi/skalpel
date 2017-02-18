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
#include "marsyas/system/MarSystemManager.h"
#include "SynthesisUtils.h"

class SinusoidalSynthSound : public SynthesiserSound
{
public:
    
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch);
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, const SineModel&);
    
    ~SinusoidalSynthSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    const SineModel::SineFrame* getFrameAt(int frame) const;
    
    void fillSpectrum(std::vector<FFT::Complex>&, int frame) const;
    
    // Get a time domain frame at a requested time location
    void getSignal(mrs_realvec&, mrs_real) const;
    
    void replaceModel(SineModel& newModel) { _model = newModel; };
    
private:
    friend class SinusoidalSynthVoice;
    
    BigInteger _midiNotes;
    int _midiRootNote;
    
    SineModel _model;
    mrs_realvec _bh1001;
    
    JUCE_LEAK_DETECTOR(SinusoidalSynthSound)
};

#endif  // SINUSOIDALSYNTHSOUND_H_INCLUDED
