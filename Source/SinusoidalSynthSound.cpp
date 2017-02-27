/*
  ==============================================================================

    SinusoidalSynthSound.cpp
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SinusoidalSynthSound.h"

// Empty model constructor
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, int frameSize)
:   _midiNotes(notes),
    _midiRootNote(midiNoteForNormalPitch),
    _frameSize(frameSize)
{
    // Create a Blackman Harris windowing for sampling
    _bh1001.create(1001);
    SynthUtils::windowingFillBlackmanHarris(_bh1001);
    
    _nyquistBin = _frameSize/2;
    
    // Allocate memory for the complex signals
    _spectrum.resize(_frameSize);
    _timeSignal.resize(_frameSize);
    
    // Inverse FFT of frame size
    _fftFunction = new FFT(std::log2(_frameSize), true);
    
    _synthWindow.create(_frameSize);
    SynthUtils::createSynthesisWindow(_synthWindow, _frameSize/4);
    
//    for (int i = 0; i < _frameSize; ++i)
//    {
//        std::cout << _synthWindow(i) << " ";
//    }
    std::cout << _synthWindow << " Synth Window\n\n";
};


// Constructor with a new model
SinusoidalSynthSound::SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch, const SineModel& model, int frameSize)
:   _midiNotes(notes),
    _midiRootNote(midiNoteForNormalPitch),
    _model(model),
    _frameSize(frameSize)
{
    // Create a Blackman Harris windowing for sampling
    _bh1001.create(1001);
    //SynthUtils::windowingFillBlackmanHarris(_bh1001);
    for (int i = 0; i < 1001; ++i)
    {
        _bh1001(i) = SynthUtils::BHCONST.at(i);
    }
    
    _nyquistBin = _frameSize/2;
    
    // Allocate memory for the complex signals
    _spectrum.resize(_frameSize);
    _timeSignal.resize(_frameSize);
    
    // Inverse FFT of frame size
    _fftFunction = new FFT(std::log2(_frameSize), true);
    
    _synthWindow.create(_frameSize);
    SynthUtils::createSynthesisWindow(_synthWindow, _frameSize/4);
    
    std::cout << _bh1001 << " Synth Window\n\n";
};


// Destructor
SinusoidalSynthSound::~SinusoidalSynthSound()
{
    delete _fftFunction;
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



bool SinusoidalSynthSound::getSignal(mrs_realvec& timeVec, mrs_real loc, int renderRate) const
{
    // Get number of frames in the model return if there aren't any
    int modelFrames = std::distance(_model.begin(), _model.end());
    if (modelFrames < 1)
    {
        return false;
    }
    
    // Get the frame closest to the requested time
    mrs_real requestedPos = (loc * _model.getSampleRate()) / _model.getFrameSize();
    int requestedFrame = std::round(requestedPos);
    mrs_real offset = (int)((requestedPos - requestedFrame) * _model.getFrameSize());
    
    // Out of frames from the model
    if (modelFrames <= requestedFrame)
    {
        return false;
    }
    
    const SineModel::SineFrame& frame = _model.getFrame(requestedFrame);
    
    std::vector<FFT::Complex> spectrum(_frameSize);
    std::vector<FFT::Complex> timeDomain(_frameSize);
    
    // Create the spectral signal
    for (auto sine = frame.begin(); sine != frame.end(); ++sine)
    {
        mrs_real binLoc =  (sine->getFreq() / renderRate) * _frameSize;
        mrs_natural binInt = std::round(binLoc);
        mrs_real binRem = binInt - binLoc;
        
        // Convert the decibels back to magnitude
        mrs_real mag = pow(10, sine->getAmp()/20);
        mrs_real phase = sine->getPhase() + 2*PI*sine->getFreq()*offset/renderRate;
        phase = std::fmod(phase, PI);
        phase = sine->getPhase();
        
        // Going to make a 9 bin wide Blackman Harris window
        if (binLoc >= 5 && binLoc < _nyquistBin-4)
        {
            for (int i = -4; i < 5; ++i)
            {
                spectrum.at(binInt + i).r += mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                spectrum.at(binInt + i).i += mag*_bh1001((int)((binRem+i)*100) + 501)*sin(phase);
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
                    spectrum.at(-1*(binInt + i)).r += mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(-1*(binInt + i)).i += -mag*_bh1001((int)((binRem+i)*100) + 501)*sin(phase);
                }
                // Real only at DC bin
                else if ((binInt + i) == 0)
                {
                    spectrum.at(0).r += 2*mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                }
                // Regular
                else
                {
                    spectrum.at(binInt + i).r += mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(binInt + i).i += mag*_bh1001((int)((binRem+i)*100) + 501)*sin(phase);
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
                    spectrum.at(_nyquistBin - (binInt + i)).r +=
                        mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(_nyquistBin - (binInt + i)).i +=
                        -mag*_bh1001((int)((binRem+i)*100) + 501)*sin(phase);
                }
                // Real only at Nyquist
                else if ((binInt + i) == _nyquistBin)
                {
                    spectrum.at(_nyquistBin).r += 2*mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                }
                // Regular
                else
                {
                    spectrum.at(binInt + i).r += mag*_bh1001((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum.at(binInt + i).i += mag*_bh1001((int)((binRem+i)*100) + 501)*sin(phase);
                }
            }
        }
    }

    mrs_real w = 440.0/renderRate*2*PI;
    mrs_real phi = loc*440.0*2*PI;
    

    // Conjugate for bins above the nyquist frequency
    for (int i = 1; i < _nyquistBin; ++i)
    {
        spectrum.at(_nyquistBin + i).r = spectrum.at(_nyquistBin - i).r;
        spectrum.at(_nyquistBin + i).i = -spectrum.at(_nyquistBin - i).i;
    }
   
//    for (auto bin = spectrum.begin(); bin != spectrum.end(); ++bin)
//    {
//        std::cout << bin->r << "+" << bin->i << "i ";
//    }
    
    //std::cout << "CalculatedEndFrame\n\n";
    
//    mrs_realvec bh;
//    bh.create(_frameSize);
//    SynthUtils::windowingFillBlackmanHarris(bh);
//    
//    std::vector<FFT::Complex> textTime(_frameSize);
//    for (int i = 0; i < _frameSize; ++i)
//    {
//        textTime.at(i).r = 0.5*sin(w*i + phi);
//    }
//    
//    FFT forwardFFT(std::log2(_frameSize), false);
//
//    forwardFFT.perform(textTime.data(), spectrum.data());
//    
//    for (auto bin = spectrum.begin(); bin != spectrum.end(); ++bin)
//    {
//        std::cout << bin->r << "+" << bin->i << "i ";
//    }
//    
//    std::cout << "EndFrame\n\n";
//    
    _fftFunction->perform(spectrum.data(), timeDomain.data());
//    

    
    for (int i = 0; i < _frameSize; ++i)
    {
        //std::cout << timeDomain.at((i + (_frameSize / 2)) % _frameSize).r / _frameSize * _synthWindow(i) << " ";
        timeVec(i) = timeDomain.at((i + (_frameSize / 2)) % _frameSize).r / _frameSize * _synthWindow(i);
        //timeVec(i) = 0.5*sin(w*i + phi) * _synthWindow(i);
        //timeVec(i) = (timeDomain.at(i).r / _frameSize) * _synthWindow(i);
        //std::cout << _synthWindow(i) << " ";
        
    }
    
    return true;
    
}
