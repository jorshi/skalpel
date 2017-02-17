/*
  ==============================================================================

    SinusoidalSynth.h
    Created: 7 Feb 2017 2:06:44pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SINUSOIDALSYNTH_H_INCLUDED
#define SINUSOIDALSYNTH_H_INCLUDED

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
    
    // Fill spectrum for a requested frame
    void fillSpectrum(std::vector<FFT::Complex>&, int frame) const;
    
    void replaceModel(SineModel& newModel) { testModel = newModel; };
    
private:
    friend class SinusoidalSynthVoice;
    
    BigInteger midiNotes;
    int midiRootNote;
    
    SineModel testModel;
    mrs_realvec bh1001;
    
    JUCE_LEAK_DETECTOR(SinusoidalSynthSound)
};


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

void cleanComplex(FFT::Complex& a);



#endif  // SINUSOIDALSYNTH_H_INCLUDED
