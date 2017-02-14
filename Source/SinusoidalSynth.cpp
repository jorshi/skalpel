/*
  ==============================================================================

    SinusoidalSynth.cpp
    Created: 7 Feb 2017 2:06:44pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynth.h"


// Constructor
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch)
        : midiNotes(notes),
          midiRootNote(midiNoteForNormalPitch)
{
    SineModel::SineFrame testFrame;
    testFrame.emplace_back(440, -6.0, 0);
    
    testModel.setFrameSize(2048);
    testModel.setSampleRate(44100.0);
    for (int i = 0; i < 100; ++i)
    {
        testModel.addFrame(testFrame);
    }
};

// Destructor
SinusoidalSynthSound::~SinusoidalSynthSound() {};

bool SinusoidalSynthSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes [midiNoteNumber];
}

bool SinusoidalSynthSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

const SineModel::SineFrame* SinusoidalSynthSound::getFrameAt(int frame) const
{
    if (frame >= std::distance(testModel.begin(), testModel.end()))
    {
        return nullptr;
    }
    
    return &testModel.getFrame(frame);
}

/*

*/

//==============================================================================
SinusoidalSynthVoice::SinusoidalSynthVoice()
: cyclesPerSample(0.0),
currentAngle(0.0),
angleDelta(0.0),
lgain (0.0f), rgain (0.0f),
attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
isInAttack (false), isInRelease (false), currentFrame(0)
{
}

SinusoidalSynthVoice::~SinusoidalSynthVoice()
{
}

bool SinusoidalSynthVoice::canPlaySound (SynthesiserSound* sound)
{
    return dynamic_cast<const SinusoidalSynthSound*> (sound) != nullptr;
}

void SinusoidalSynthVoice::startNote (const int midiNoteNumber,
                              const float velocity,
                              SynthesiserSound* s,
                              const int /*currentPitchWheelPosition*/)
{
    if (const SinusoidalSynthSound* const sound = dynamic_cast<const SinusoidalSynthSound*> (s))
    {
        currentFrame = 0;
        //cyclesPerSample = pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) * 440 / getSampleRate();
        //angleDelta = cyclesPerSample * 2.0 * double_Pi;
    }
    else
    {
        jassertfalse;
    }
}

void SinusoidalSynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    clearCurrentNote();
    currentFrame = 0;
    //angleDelta = 0.0;
}

void SinusoidalSynthVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void SinusoidalSynthVoice::controllerMoved (const int /*controllerNumber*/,
                                    const int /*newValue*/)
{
}

//==============================================================================
void SinusoidalSynthVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    
    if (const SinusoidalSynthSound* const playingSound = static_cast<SinusoidalSynthSound*> (getCurrentlyPlayingSound().get()))
    {
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        updateWindow(outputBuffer.getNumSamples());
        
        const float level = 0.125f;
        
        const SineModel::SineFrame* const frameModel = playingSound->getFrameAt(currentFrame);
        
        if (frameModel == nullptr)
        {
            clearCurrentNote();
        }
        
        ++currentFrame;
        
    }
}


void SinusoidalSynthVoice::updateWindow(mrs_natural size)
{
    // Cached triangle window is already the corrent size
    if (size == _window.getSize())
        return;
    
    // Blackman Harris Window
    mrs_realvec bhWindow;
    bhWindow.create(size);
    windowingFillBlackmanHarris(bhWindow);
    
    // Triangle Window
    mrs_realvec tWindow;
    tWindow.create(size);
    windowingFillTriangle(tWindow);
    
    mrs_real bSum = bhWindow.sum();
    for (int i = 0; i < size; ++i)
    {
        
    }
    
    
    
}


void SinusoidalSynthVoice::windowingFillBlackmanHarris(realvec& envelope)
{
    mrs_natural N = envelope.getSize();
    mrs_real a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
    for (mrs_natural t = 0; t < N; t++)
    {
        envelope(t) = a0  - a1 * cos(2.0 * PI * t / (N - 1.0))
        + a2 * cos(4.0 * PI * t / (N - 1.0))
        - a3 * cos(6.0 * PI * t / (N - 1.0));
    }
}



void SinusoidalSynthVoice::windowingFillTriangle(realvec& envelope)
{
    mrs_natural N = envelope.getSize();
    for (mrs_natural t = 0; t < N; t++)
    {
        envelope(t) = 2.0/N * (N/2.0 - std::abs(t - (N - 1.0)/2.0));
    }
}

