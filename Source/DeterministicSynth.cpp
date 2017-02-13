/*
  ==============================================================================

    DeterministicSynth.cpp
    Created: 7 Feb 2017 2:06:44pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "DeterministicSynth.h"

// Constructor
DeterministicSound::DeterministicSound(const BigInteger& notes, int midiNoteForNormalPitch)
        : midiNotes(notes),
          midiRootNote(midiNoteForNormalPitch)
{
};

// Destructor
DeterministicSound::~DeterministicSound() {};

bool DeterministicSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes [midiNoteNumber];
}

bool DeterministicSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

/*
    Deterministic Voice
*/

//==============================================================================
DeterministicVoice::DeterministicVoice()
: cyclesPerSample(0.0),
currentAngle(0.0),
angleDelta(0.0),
lgain (0.0f), rgain (0.0f),
attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
isInAttack (false), isInRelease (false)
{
}

DeterministicVoice::~DeterministicVoice()
{
}

bool DeterministicVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<const DeterministicSound*> (sound) != nullptr;
}

void DeterministicVoice::startNote (const int midiNoteNumber,
                              const float velocity,
                              SynthesiserSound* s,
                              const int /*currentPitchWheelPosition*/)
{
    if (const DeterministicSound* const sound = dynamic_cast<const DeterministicSound*> (s))
    {
        cyclesPerSample = pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
            * 440 / getSampleRate();
        angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void DeterministicVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    clearCurrentNote();
    angleDelta = 0.0;
}

void DeterministicVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void DeterministicVoice::controllerMoved (const int /*controllerNumber*/,
                                    const int /*newValue*/)
{
}

//==============================================================================
void DeterministicVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    
    if (const DeterministicSound* const playingSound = static_cast<DeterministicSound*> (getCurrentlyPlayingSound().get()))
    {
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        const float level = 0.125f;
        
        while (--numSamples >= 0)
        {
            const float currentSample = (float) std::sin(currentAngle) * level;
            currentAngle += angleDelta;
            
            if (outR != nullptr)
            {
                *outL++ += currentSample;
                *outR++ += currentSample;
            }
            else
            {
                *outL++ += currentSample;
            }
        }
    }
}
