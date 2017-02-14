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
#include "marsyas/system/MarSystem.h"


class SinusoidalSynthSound : public SynthesiserSound
{
public:
    
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch);
    
    ~SinusoidalSynthSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    const SineModel::SineFrame* getFrameAt(int frame) const;
    
private:
    friend class SinusoidalSynthVoice;
    
    BigInteger midiNotes;
    int midiRootNote;
    
    SineModel testModel;
    
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
    
    void updateWindow(mrs_natural size);
    
    
    // Marsyas fill Blackman Harris window
    void windowingFillBlackmanHarris(realvec& envelope);
    
    // Marsyas fill Triangle Window
    void windowingFillTriangle(realvec& envelope);
    
    
private:
    //==============================================================================
    double cyclesPerSample;
    double currentAngle;
    double angleDelta;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
    int currentFrame;
    mrs_realvec _window;
    
    JUCE_LEAK_DETECTOR (SinusoidalSynthVoice)
};



#endif  // SINUSOIDALSYNTH_H_INCLUDED
