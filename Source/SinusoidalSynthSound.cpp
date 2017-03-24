/*
  ==============================================================================

    SinusoidalSynthSound.cpp
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthSound.h"

// Empty model constructor
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch,
                                           int frameSize, SoundInterfaceManager* manager)
:   _midiNotes(notes),
    _midiRootNote(midiNoteForNormalPitch),
    frameSize_(frameSize),
    manager_(manager)
{
    bh1001_.create(1001);
    //SynthUtils::windowingFillBlackmanHarris(_bh1001);
    for (int i = 0; i < 1001; ++i)
    {
        bh1001_(i) = SynthUtils::BHCONST.at(i);
    }
    
    _nyquistBin = frameSize_/2;
    
    // Allocate memory for the complex signals
    _spectrum.resize(frameSize_);
    _timeSignal.resize(frameSize_);
    
    // Inverse FFT of frame size
    _fftFunction = new FFT(std::log2(frameSize_), true);
    
    synthWindow_.create(frameSize_);
    SynthUtils::createSynthesisWindow(synthWindow_, frameSize_/4);
};


// Destructor
SinusoidalSynthSound::~SinusoidalSynthSound()
{
};


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


// Get signal: returns a time domain signal of the sine model at the requested location
bool SinusoidalSynthSound::getSignal(mrs_realvec& timeVec, mrs_real loc, int renderRate) const
{
    
    ReferenceCountedArray<SineModel> activeModels = getPlayingSineModels();
    if (activeModels.size() < 1)
    {
        return false;
    }
    
    SineModel::ConstPtr model = activeModels[0];
    
    // Get number of frames in the model return if there aren't any
    int modelFrames = std::distance(model->begin(), model->end());
    if (modelFrames < 1)
    {
        return false;
    }
    
    // Get the frame closest to the requested time
    mrs_real requestedPos = (loc * model->getSampleRate()) / model->getFrameSize();
    int requestedFrame = std::round(requestedPos);
    
    // Out of frames from the model
    if (modelFrames <= requestedFrame)
    {
        return false;
    }
    
    // Constant reference to the frame at this point
    const SineModel::SineFrame& frame = model->getFrame(requestedFrame);
    
    std::vector<FFT::Complex> spectrum(frameSize_);
    std::vector<FFT::Complex> timeDomain(frameSize_);
    
    // Create the spectral signal
    for (auto sine = frame.begin(); sine != frame.end(); ++sine)
    {
        mrs_real binLoc =  (sine->getFreq() / renderRate) * frameSize_;
        mrs_natural binInt = std::round(binLoc);
        mrs_real binRem = binInt - binLoc;
        
        // Convert the decibels back to magnitude
        mrs_real mag = pow(10, sine->getAmp()/20);
        mrs_real phase = sine->getPhase();
        
        // Going to make a 9 bin wide Blackman Harris window
        if (binLoc >= 5 && binLoc < _nyquistBin-4)
        {
            for (int i = -4; i < 5; ++i)
            {
                spectrum.at(binInt + i).r += mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                spectrum.at(binInt + i).i += mag*bh1001_((int)((binRem+i)*100) + 501)*sin(phase);
            }
        }
        // Some components will wrap around 0
        else if (binLoc < 5 && binLoc > 0)
        {
            for (int i = -4; i < 5; ++i)
            {
                // Complex Conjugate wraps around DC bin
                if ((binInt + i) < 0)
                {
                    spectrum.at(-1*(binInt + i)).r += mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(-1*(binInt + i)).i += -mag*bh1001_((int)((binRem+i)*100) + 501)*sin(phase);
                }
                // Real only at DC bin
                else if ((binInt + i) == 0)
                {
                    spectrum.at(0).r += 2*mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                }
                // Regular
                else
                {
                    spectrum.at(binInt + i).r += mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(binInt + i).i += mag*bh1001_((int)((binRem+i)*100) + 501)*sin(phase);
                }
            }
        }
        // Wrap around the Nyquist bin
        else if (binLoc < (_nyquistBin - 1) && binLoc >= (_nyquistBin-4))
        {
            for (int i = -4; i < 5; ++i)
            {
                // Complex Conjugate wraps nyquist bin
                if ((binInt + i) > _nyquistBin)
                {
                    spectrum.at((binInt + i) - _nyquistBin).r +=
                        mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at((binInt + i) - _nyquistBin).i +=
                        -mag*bh1001_((int)((binRem+i)*100) + 501)*sin(phase);
                }
                // Real only at Nyquist
                else if ((binInt + i) == _nyquistBin)
                {
                    spectrum.at(_nyquistBin).r += 2*mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                }
                // Regular
                else
                {
                    spectrum.at(binInt + i).r += mag*bh1001_((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(binInt + i).i += mag*bh1001_((int)((binRem+i)*100) + 501)*sin(phase);
                }
            }
        }
    }

    // Conjugate for bins above the nyquist frequency
    for (int i = 1; i < _nyquistBin; ++i)
    {
        spectrum.at(_nyquistBin + i).r = spectrum.at(_nyquistBin - i).r;
        spectrum.at(_nyquistBin + i).i = -spectrum.at(_nyquistBin - i).i;
    }
   
    _fftFunction->perform(spectrum.data(), timeDomain.data());
  
    // Apply synthesis window & shift
    for (int i = 0; i < frameSize_; ++i)
    {
        timeVec(i) = timeDomain.at((i + (frameSize_ / 2)) % frameSize_).r / frameSize_ * synthWindow_(i);
    }
    
    return true;
}


ReferenceCountedArray<SineModel> SinusoidalSynthSound::getPlayingSineModels() const
{
    ReferenceCountedArray<SineModel> models;
    Array<SoundInterface*> activeSounds = manager_->getActiveSounds();
    
    for (int i = 0; i < activeSounds.size(); i++)
    {
        models.add(activeSounds[i]->getSineModel());
    }

    return models;
}

