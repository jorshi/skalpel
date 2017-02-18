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
ifft(log2(512), true)
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
        
        
        
        
        //        while (numSamples > 0)
        //        {
        //            // Fill output
        //            if (_hopIndex < _hopSize)
        //            {
        //                *outL = 0.0;
        //                for (int i = 0; i < 4; ++i)
        //                {
        //                    int frameNum = (_overlapIndex - i + 4) % 4;
        //                    int index = _hopIndex + (_hopSize*i);
        //                    *outL += _frames.at((_overlapIndex - i + 4) % 4).at(_hopIndex + (_hopSize*i)).r;
        //                }
        //                *outR = *outL;
        //
        //                std::cout << *outL << " ";
        //
        //                ++outL;
        //                ++outR;
        //                ++_hopIndex;
        //                --numSamples;
        //            }
        //
        //            // Need to calculate more windows
        //            else
        //            {
        //                // Increment overlap index
        //                _overlapIndex = (_overlapIndex + 1) % 4;
        //
        //                // Get next spectral frame
        //                playingSound->fillSpectrum(_spectrum, currentFrame);
        //
        //
        //                //for (auto bin = _spectrum.begin(); bin != _spectrum.end(); ++bin)
        //                //{
        //                //    std::cout << bin->r << "+j" << bin->i << " ";
        //                //}
        //
        //                if (_spectrum.size() == 0)
        //                {
        //                    // No more samples to render! Clear note and set samples to zero
        //                    clearCurrentNote();
        //
        //                    // This is annoying!! TODO fix this
        //                    _spectrum.resize(512);
        //                    currentFrame = 0;
        //                    _hopIndex = _hopSize;
        //
        //                    // Need to clear out all the samples!
        //                    for (auto frame = _frames.begin(); frame != _frames.end(); ++frame)
        //                    {
        //                        std::for_each(frame->begin(), frame->end(), cleanComplex);
        //                    }
        //                } else {
        //
        //                    std::for_each(_frames.at(_overlapIndex).begin(),
        //                                  _frames.at(_overlapIndex).end(),
        //                                  cleanComplex);
        //                    ifft.perform(_spectrum.data(), _frames.at(_overlapIndex).data());
        //
        //                    std::rotate(_frames.at(_overlapIndex).begin(),
        //                                _frames.at(_overlapIndex).begin()+256,
        //                                _frames.at(_overlapIndex).end());
        //                    
        //                    
        //                    for (int i = 0; i < _frames.at(_overlapIndex).size(); ++i)
        //                    {
        //                        _frames.at(_overlapIndex).at(i).r *= _synthWindow(i);
        //                    }
        //                }
        //                
        //                _hopIndex = 0;
        //            }
        //        }
        
        
        std::cout << "end of samples\n\n";
        
        //std::cout << _synthWindow << "\n";
        
        
        ++currentFrame;
    }
}

