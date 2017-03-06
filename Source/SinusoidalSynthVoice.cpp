/*
  ==============================================================================

    SinusoidalSynthVoice.cpp
    Created: 17 Feb 2017 4:03:11pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthVoice.h"


// Default Constructor
SinusoidalSynthVoice::SinusoidalSynthVoice()
:
_hopSize(128),
_windowSize(512),
_overlapIndex(0),
_location(0.0)
{
    _hopIndex = _hopSize;
    _writePos = 0;
    _readPos = 0;
}

// Deconstructor
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
        _hopSize = sound->getFrameSize() / 4;
        _buffer.create(sound->getFrameSize());
        
        _location = 0.0;
        _readPos = 0;
        _writePos = 0;
        _hopIndex = _hopSize;
    }
    else
    {
        jassertfalse;
    }
}

void SinusoidalSynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    _hopIndex = _hopSize;
    _overlapIndex = 0;
    _location = 0.0;
    _readPos = 0;
    clearCurrentNote();
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
        
        int numCalculated = 0;
        
        
        while (numCalculated < numSamples)
        {
            // Get a new frame
            if (_hopIndex >= _hopSize)
            {
                mrs_realvec output(playingSound->getFrameSize());
                
                if (playingSound->getSignal(output, _location, getSampleRate()))
                {
                    // Do overlap add
                    for (int i = 0; i < playingSound->getFrameSize(); ++i)
                    {
                        if (i < _hopSize)
                        {
                            _buffer((_writePos + i) % playingSound->getFrameSize()) = 0.0;
                        }
                        else
                        {
                            _buffer((_writePos + i) % playingSound->getFrameSize()) += output(i);
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < _hopSize; ++i)
                    {
                        _buffer(_writePos + i) = 0.0;
                    }
                    clearCurrentNote();
                }
                
                // Update write pointer and read pointer
                _writePos = (_writePos + _hopSize) % playingSound->getFrameSize();
                _readPos = (_readPos + _hopSize) % playingSound->getFrameSize();
                _location += (_hopSize / getSampleRate());
                _hopIndex = 0;
            }
            else
            {
                *(outL + numCalculated) = _buffer(_readPos + _hopIndex);
                *(outR + numCalculated) = _buffer(_readPos + _hopIndex);
                numCalculated++;
                _hopIndex++;
            }
        }
    }
}
