/*
  ==============================================================================

    DeterministicSynth.h
    Created: 7 Feb 2017 2:06:44pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef DETERMINISTICSYNTH_H_INCLUDED
#define DETERMINISTICSYNTH_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class DeterministicSound : public SynthesiserSound
{
public:
    
    DeterministicSound(const BigInteger& notes, int midiNoteForNormalPitch);
    
    ~DeterministicSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
private:
    friend class DeterministicVoice;
    
    BigInteger midiNotes;
    int midiRootNote;
    
    JUCE_LEAK_DETECTOR(DeterministicSound)
};


class DeterministicVoice    : public SynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    DeterministicVoice();
    
    /** Destructor. */
    ~DeterministicVoice();
    
    //==============================================================================
    bool canPlaySound (SynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    
private:
    //==============================================================================
    double cyclesPerSample;
    double currentAngle;
    double angleDelta;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
    
    JUCE_LEAK_DETECTOR (DeterministicVoice)
};



#endif  // DETERMINISTICSYNTH_H_INCLUDED
