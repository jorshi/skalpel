/*
  ==============================================================================

    SinusoidalSynthSound.cpp
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthSound.h"

// Empty model constructor
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch)
:   _midiNotes(notes),
    _midiRootNote(midiNoteForNormalPitch)

{
    // Create a Blackman Harris windowing for sampling
    _bh1001.create(1001);
    SynthUtils::windowingFillBlackmanHarris(_bh1001);
};


// Constructor with a new model
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, const SineModel& model)
:   _midiNotes(notes),
    _midiRootNote(midiNoteForNormalPitch),
    _model(model)
{
    // Create a Blackman Harris windowing for sampling
    _bh1001.create(1001);
    SynthUtils::windowingFillBlackmanHarris(_bh1001);
};


// Destructor
SinusoidalSynthSound::~SinusoidalSynthSound() {};


// Sound for note?
bool SinusoidalSynthSound::appliesToNote (int midiNoteNumber)
{
    return _midiNotes [midiNoteNumber];
}


// Sound for midi channel?
bool SinusoidalSynthSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}


// DELETE
const SineModel::SineFrame* SinusoidalSynthSound::getFrameAt(int frame) const
{
    if (frame >= std::distance(testModel.begin(), testModel.end()))
    {
        return nullptr;
    }
    
    return &testModel.getFrame(frame);
}


void SinusoidalSynthSound::getSignal(mrs_realvec& timeVec, mrs_real loc) const
{
    // Get number of frames in the model return if there aren't any
    int modelFrames = std::distance(_model.begin(), _model.end());
    if (modelFrames < 1)
    {
        return;
    }
    
    // Get the frame closest to the requested time
    int requestedFrame = std::round((loc * _model.getSampleRate()) / _model.getFrameSize());
    
    // Out of frames from the model
    if (modelFrames <= requestedFrame)
    {
        return;
    }
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
