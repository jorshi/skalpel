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
        
    
    
    
    
}



/*

*/

//==============================================================================
SinusoidalSynthVoice::SinusoidalSynthVoice()
: currentFrame(0), ifft(log2(512), true)
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
        
        
        std::vector<FFT::Complex> spectralFrame(512);
        playingSound->fillSpectrum(spectralFrame, currentFrame);
        
        if (spectralFrame.size() == 0)
        {
            clearCurrentNote();
        }
        
        std::vector<FFT::Complex> timeDomain(512);
        ifft.perform(spectralFrame.data(), timeDomain.data());
        
        //for (auto sample = timeDomain.begin(); sample != timeDomain.end(); ++sample)
        //{
        //    *outL = sample->r;
        //    *outR = sample->r;
        //    ++outL;
        //    ++outR;
        //}
        
        
        ++currentFrame;
    }
}




