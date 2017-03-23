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
#include "SoundInterface.h"


class SinusoidalSynthSound : public SynthesiserSound
{
public:
    
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, int frameSize);
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, const SineModel&, int frameSize);
    
    ~SinusoidalSynthSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    const int& getFrameSize() const { return _frameSize; };
    
    // Get a time domain frame at a requested time location
    bool getSignal(mrs_realvec&, mrs_real, int) const;
    
    // Switch out the Sine Model being rendered
    void replaceModel(SineModel& newModel) { _model = newModel; };
    
    // Point to a new sinusoidal model to include in synthesis
    void addModel(const SineModel* newModel, int soundNum);
    
    // Remove pointer to one of the sound models
    void removeModel(int soundNum);
    
    
private:
    friend class SinusoidalSynthVoice;
    
    // Reset all real and imaginary spectrum values to 0
    void resetSpectrum();
    
    BigInteger _midiNotes;
    int _midiRootNote;
    
    SineModel _model;
    mrs_realvec _bh1001;
    mrs_realvec _synthWindow;

    int _frameSize;
    mrs_real _nyquistBin;
    
    // Signals for holding spectral and time domain signals
    std::vector<FFT::Complex> _spectrum;
    std::vector<FFT::Complex> _timeSignal;
    
    // Sine models representing this sound
    std::vector<const SineModel*> sineModels_;
    
    // Pointer to FFT class
    ScopedPointer<FFT> _fftFunction;
    
    JUCE_LEAK_DETECTOR(SinusoidalSynthSound)
};

#endif  // SINUSOIDALSYNTHSOUND_H_INCLUDED
