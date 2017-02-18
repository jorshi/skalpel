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
    //=============================================================================
    int currentFrame;
    
    int _hopSize;
    int _hopIndex;
    int _windowSize;
    int _overlapIndex;
    
    mrs_realvec _window;
    mrs_realvec _synthWindow;
    
    std::vector<std::vector<FFT::Complex>> _frames;
    std::vector<FFT::Complex> _spectrum;
    
    FFT ifft;
    
    JUCE_LEAK_DETECTOR (SinusoidalSynthVoice)
};

#endif  // SINUSOIDALSYNTHVOICE_H_INCLUDED
