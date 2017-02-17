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
: midiNotes(notes), midiRootNote(midiNoteForNormalPitch)

{
    SineModel::SineFrame testFrame;
    testFrame.emplace_back(440, -6.0, 0);
    
    testModel.setFrameSize(2048);
    testModel.setSampleRate(44100.0);
    for (int i = 0; i < 100; ++i)
    {
        testModel.addFrame(testFrame);
    }
    
    // Create a Blackman Harris windowing for sampling
    bh1001.create(1001);
    SynthUtils::windowingFillBlackmanHarris(bh1001);
    
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

void SinusoidalSynthSound::fillSpectrum(std::vector<FFT::Complex>& spectrum, int frame) const
{
    if (frame >= std::distance(testModel.begin(), testModel.end()))
    {
        spectrum.clear();
        return;
    }
    
    const SineModel::SineFrame& sineFrame = testModel.getFrame(frame);
    const mrs_real modelRate = testModel.getSampleRate();
    const mrs_real nyquistBin = spectrum.size() / 2;
    
    
    // Create the spectral signal
    for (auto sine = sineFrame.begin(); sine != sineFrame.end(); ++sine)
    {
        mrs_real binLoc =  (sine->getFreq() / modelRate) * spectrum.size();
        mrs_natural binInt = std::round(binLoc);
        mrs_real binRem = binLoc - binInt;
        
        // Convert the decibels back to magnitude
        mrs_real mag = pow(10, sine->getAmp()/20);
        
        // Going to make a 9 bin wide Blackman Harris window
        if (binLoc >= 5 && binLoc < nyquistBin-4)
        {
            for (int i = -4; i < 5; ++i)
            {
                spectrum.at(binInt + i).r += mag*bh1001((int)((binRem+i)*100) + 501)*cos(sine->getPhase());
                spectrum.at(binInt + i).i += mag*bh1001((int)((binRem+i)*100) + 501)*sin(sine->getPhase());
            }
        }
    }
    
    
    for (int i = 1; i < nyquistBin; ++i)
    {
        spectrum.at(nyquistBin + i).r = spectrum.at(nyquistBin - i).r;
        spectrum.at(nyquistBin + i).i = -spectrum.at(nyquistBin - i).i;
    }
    
    // This should totally compute the time domain signal and apply the windowing
}



/*

*/

//==============================================================================
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
    SynthUtils::createSynthesisWindow(_synthWindow);
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
    
    for (auto frame = _frames.begin(); frame != _frames.end(); ++frame)
    {
        std::for_each(frame->begin(), frame->end(), cleanComplex);
    }
    
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
        
       
        while (numSamples > 0)
        {
            // Fill output
            if (_hopIndex < _hopSize)
            {
                *outL = 0.0;
                for (int i = 0; i < 4; ++i)
                {
                    *outL += _frames.at((_overlapIndex - i + 4) % 4).at(_hopIndex + (_hopSize*i)).r;
                }
                *outR = *outL;
                
                ++outL;
                ++outR;
                ++_hopIndex;
                --numSamples;
            }
            
            // Need to calculate more windows
            else
            {
                // Increment overlap index
                _overlapIndex = (_overlapIndex + 1) % 4;
                
                // Get next spectral frame
                playingSound->fillSpectrum(_spectrum, currentFrame);
                
                
                if (_spectrum.size() == 0)
                {
                    // No more samples to render! Clear note and set samples to zero
                    clearCurrentNote();
                    
                    // This is annoying!! TODO fix this
                    _spectrum.resize(512);
                    currentFrame = 0;
                    
                    // Need to clear out all the samples!
                    std::for_each(_frames.at(_overlapIndex).begin(),
                                  _frames.at(_overlapIndex).end(),
                                  cleanComplex);
                } else {
                
                    ifft.perform(_spectrum.data(), _frames.at(_overlapIndex).data());
                    
                    std::rotate(_frames.at(_overlapIndex).begin(),
                                _frames.at(_overlapIndex).begin()+256,
                                _frames.at(_overlapIndex).end());
                    
                    for (int i = 0; i < _frames.at(_overlapIndex).size(); ++i)
                    {
                        _frames.at(_overlapIndex).at(i).r *= _synthWindow(i);
                    }
                }
                
                _hopIndex = 0;
            }
        }

        
        ++currentFrame;
    }
}

void cleanComplex(FFT::Complex& a)
{
    a.r = 0.0;
    a.i = 0.0;
}




