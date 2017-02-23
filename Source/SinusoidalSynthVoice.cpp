/*
  ==============================================================================

    SinusoidalSynthVoice.cpp
    Created: 17 Feb 2017 4:03:11pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthVoice.h"


SinusoidalSynthVoice::SinusoidalSynthVoice()
:   currentFrame(0),
_hopSize(128),
_windowSize(512),
_overlapIndex(0),
_spectrum(512),
_location(0.0)
{
    // Create the synthesis window
    _synthWindow.create(512);
    SynthUtils::createSynthesisWindow(_synthWindow, _hopSize);
    std::cout << _synthWindow << "\n";
    
    // Setup windows for overlap add
    for (int i = 0; i < 4; ++i)
    {
        _frames.emplace_back(std::vector<FFT::Complex>(_windowSize));
    }
    
    _hopIndex = _hopSize;
    _writePos = 0;
    _readPos = 0;
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
        _hopSize = sound->getFrameSize() / 4;
        _buffer.create(sound->getFrameSize());
        
        currentFrame = 0;
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
    clearCurrentNote();
    currentFrame = 0;
    _hopIndex = _hopSize;
    _overlapIndex = 0;
    _spectrum.resize(512);
    _location = 0.0;
    _readPos = 0;
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
        
        const float level = 0.125f;
        
        
        int numCalculated = 0;
        
        while (numCalculated < numSamples)
        {
            // Get a new frame
            if (_hopIndex >= _hopSize)
            {
                mrs_realvec output;
                output.allocate(playingSound->getFrameSize());
                
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
                    clearCurrentNote();
                    for (int i = 0; i < _hopSize; ++i)
                    {
                        _buffer(_writePos + i) = 0.0;
                    }
                }
                
                // Update write pointer and read pointer
                _writePos = (_writePos + _hopSize) % playingSound->getFrameSize();
                _readPos = (_readPos + _hopSize) % playingSound->getFrameSize();
                
                _location += (_hopSize / getSampleRate());
                _hopIndex = 0;
            }
            else
            {
                *(outL + numCalculated + startSample) = _buffer(_readPos + _hopIndex);
                *(outR + numCalculated + startSample) = _buffer(_readPos + _hopIndex);
                numCalculated++;
                _hopIndex++;
            }
            
        }
        
        for (int i = 0; i < numSamples; ++i)
        {
            //std::cout << *(outL + startSample + i) << " ";
        }
        
        //std::cout << "Rendered Samples\n";
    }
    
}

