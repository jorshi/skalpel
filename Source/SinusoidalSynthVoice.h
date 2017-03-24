/*
  ==============================================================================

    SinusoidalSynthVoice.h
    Created: 17 Feb 2017 4:03:11pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SINUSOIDALSYNTHVOICE_H_INCLUDED
#define SINUSOIDALSYNTHVOICE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SineElement.h"
#include "marsyas/system/MarSystemManager.h"
#include "SynthesisUtils.h"
#include "SinusoidalSynthSound.h"

class SinusoidalSynthVoice : public SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a Sinusoidal Synth Voice. */
    SinusoidalSynthVoice();
    
    /** Destructor. */
    ~SinusoidalSynthVoice();
    
    //==============================================================================
    bool canPlaySound (SynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;

private:
    
    bool renderFrames (mrs_realvec& buffer, const SinusoidalSynthSound* const sound);
    
    
    //=============================================================================
    int hopSize_;
    int frameSize_;
    int hopIndex_;
    int overlapIndex_;
    int readPos_;
    int writePos_;
    int nyquistBin_;
    
    mrs_real location_;
    mrs_realvec buffer_;
    
    ScopedPointer<FFT> inverseFFT_;
    
    
    JUCE_LEAK_DETECTOR (SinusoidalSynthVoice)
};

#endif  // SINUSOIDALSYNTHVOICE_H_INCLUDED
