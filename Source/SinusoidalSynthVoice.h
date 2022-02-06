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
#include "SynthesisUtils.h"
#include "SinusoidalSynthSound.h"
#include "SynthesisParameterManager.h"
#include "SoundInterfaceManager.h"
#include "Modulation.h"
#include "ADSREnv.h"
#include "ModulationFactory.h"
#include "StochasticModel.h"

class SinusoidalSynthVoice : public SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a Sinusoidal Synth Voice. */
    SinusoidalSynthVoice(SoundInterfaceManager& s);
    
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
    
    struct PrevElement
    {
        double freq;
        double phase;
        
        PrevElement(double f, double p)
        {
            freq = f;
            phase = p;
        }
    };
    
    bool renderFrames (std::vector<double>& buffer,
                       const SinusoidalSynthSound* const sound);
    
    void releaseOver();
    
    
    //=============================================================================
    int hopSize_;
    int frameSize_;
    int hopIndex_;
    int overlapIndex_;
    int readPos_;
    int writePos_;
    int nyquistBin_;
    
    std::vector<double> buffer_;
    std::vector<double> output_;
    double noteFreqScale_;
    
    std::vector<float> location_;
    
    // FFTs for Sine Modelling
    std::unique_ptr<dsp::FFT> inverseFFT_;
    std::vector<dsp::Complex<float>> spectrum_;
    std::vector<dsp::Complex<float>> timeDomain_;
    
    ReferenceCountedArray<SineModel> activeModels_;
    ReferenceCountedArray<StochasticModel> activeNoiseModels_;
    
    Array<SynthesisParameterManager*> params_;
    Array<SoundInterface*> activeSounds_;
    SoundInterfaceManager& soundManger_;
    
    Array<bool> isSoundActive_;
    
    std::vector<std::map<int, PrevElement>> previousElements_;

    Modulation::Ptr env1_;
    
    Random random_;
    
    JUCE_LEAK_DETECTOR (SinusoidalSynthVoice)
};

#endif  // SINUSOIDALSYNTHVOICE_H_INCLUDED
