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
hopSize_(128),
frameSize_(512),
overlapIndex_(0),
location_(0.0)
{
    hopIndex_ = hopSize_;
    writePos_ = 0;
    readPos_ = 0;
    nyquistBin_ = frameSize_ / 2;
    
    // Inverse FFT of frame size
    inverseFFT_ = new FFT(std::log2(frameSize_), true);
    
    spectrum_.resize(frameSize_);
    timeDomain_.resize(frameSize_);
    
    output_.create(frameSize_);
    buffer_.create(frameSize_);
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
        location_ = 0.0;
        readPos_ = 0;
        writePos_ = 0;
        hopIndex_ = hopSize_;
        
        // Models producing sound
        activeModels_ = sound->getPlayingSineModels();
        previousElements_.resize(activeModels_.size());
        
        noteFreqScale_ = pow(2.0, (midiNoteNumber - sound->midiRootNote_)/12.0f);
        
        env1_ = ModulationFactory::make("adsr_1");
        ADSR* adsr;
        if ((adsr = dynamic_cast<ADSR*>(env1_.get())))
        {
            adsr->setPhase(ADSR::attack);
        }
    }
    else
    {
        jassertfalse;
    }
}

void SinusoidalSynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    ADSR* adsr;
    if ((adsr = dynamic_cast<ADSR*>(env1_.get())))
    {
        adsr->turnOff();
    }
    
    activeModels_.clear();
    previousElements_.clear();
    hopIndex_ = hopSize_;
    overlapIndex_ = 0;
    location_ = 0.0;
    readPos_ = 0;
    buffer_.setval(0.0);
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
        if (activeModels_.size() < 1)
        {
            return;
        }
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        int numCalculated = 0;
        
        while (numCalculated < numSamples)
        {
            // Get a new frame
            if (hopIndex_ >= hopSize_)
            {
                if (renderFrames(output_, playingSound))
                {
                    // Do overlap add
                    for (int i = 0; i < playingSound->getFrameSize(); ++i)
                    {
                        if (i < hopSize_)
                        {
                            buffer_((writePos_ + i) % playingSound->getFrameSize()) = 0.0;
                        }
                        else
                        {
                            buffer_((writePos_ + i) % playingSound->getFrameSize()) += output_(i);
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < hopSize_; ++i)
                    {
                        buffer_(writePos_ + i) = 0.0;
                    }
                    clearCurrentNote();
                }
                
                // Update write pointer and read pointer
                writePos_ = (writePos_ + hopSize_) % playingSound->getFrameSize();
                readPos_ = (readPos_ + hopSize_) % playingSound->getFrameSize();
                location_ += (hopSize_ / getSampleRate());
                hopIndex_ = 0;
            }
            else
            {
                *(outL + numCalculated) += buffer_(readPos_ + hopIndex_);
                *(outR + numCalculated) += buffer_(readPos_ + hopIndex_);
                numCalculated++;
                hopIndex_++;
            }
        }
    }
}

//==============================================================================
bool SinusoidalSynthVoice::renderFrames(mrs_realvec &buffer, const SinusoidalSynthSound* const sound)
{
    if (activeModels_.size() < 1)
    {
        return false;
    }
    
    // Zero out first half of spectrum
    std::for_each(spectrum_.begin(), spectrum_.begin() + nyquistBin_, [](FFT::Complex& complex){
        complex.r = 0;
        complex.i = 0;
    });
    
    // Declare some variables for use in processing loop
    mrs_real freq;
    mrs_real binLoc;
    mrs_natural binInt;
    mrs_real binRem;
    
    float mag;
    float ampEnv = 1.0f;
    mrs_real phase;

    std::map<int, PrevElement>::iterator prev;

    for (int modelNum = 0; modelNum < activeModels_.size(); modelNum++)
    {
        SineModel::ConstPtr model = activeModels_[0];
        
        // Get number of frames in the model return if there aren't any
        if (model->size() < 1)
        {
            return false;
        }
        
        // Replace the float factor with a parameter, between 0 and 1
        mrs_real startTimeOffset = 0.0f * model->size();
        
        // Get the frame closest to the requested time, float factor to be parameterizeda
        mrs_real requestedPos = (1.0f * location_ * model->getSampleRate()) / model->getFrameSize();
        int requestedFrame = std::round(requestedPos + startTimeOffset);
        
        // Out of frames from the model
        if (model->size() <= requestedFrame)
        {
            return false;
        }
        
        // Amplitude envelope value to be applied at this frame
        env1_->apply(ampEnv);
        
        // Constant reference to the frame at this point
        const SineModel::SineFrame& frame = model->getFrame(requestedFrame);
        
        // Create the spectral signal
        for (auto sine = frame.begin(); sine != frame.end(); ++sine)
        {
            // Do frequency transformations here
            freq = sine->getFreq();
            
            // Frequency Scaling
            freq *= noteFreqScale_;
            
            // Frequency Stretching
            float stretchRatio = freq / 440.0;
            float stretchFactor = 0.0f;
            
            if (stretchFactor > 0.0f)
            {
                if (stretchRatio > 1)
                {
                    freq = pow(freq, (stretchFactor * stretchRatio) + 1.0f);
                }
                else if (stretchRatio < 1)
                {
                    freq = pow(freq, 1.0f - (stretchFactor * stretchRatio));
                }
            }
            
            // Frequency Shifting
            float freqShift = 0.0f;
            freq += freqShift;
            
            
            binLoc =  (freq / getSampleRate()) * frameSize_;
            binInt = std::round(binLoc);
            binRem = binInt - binLoc;
            
            // Convert the decibels back to magnitude. Gain should be parameterized.
            float gain = -6.0f;
            mag = pow(10, (sine->getAmp() + gain)/20) * ampEnv;
            
            // Propagate phase
            if ((prev = previousElements_.at(0).find(sine->getTrack())) ==  previousElements_.at(0).end())
            {
                phase = sine->getPhase();
                previousElements_.at(0).emplace(sine->getTrack(), PrevElement(freq, sine->getPhase()));
            }
            else
            {
                prev->second.phase += (PI * (prev->second.freq + freq) / getSampleRate()) * hopSize_;
                prev->second.freq = freq;
                phase = prev->second.phase;
            }
            
            // Going to make a 9 bin wide Blackman Harris window
            if (binLoc >= 5 && binLoc < nyquistBin_-4)
            {
                for (int i = -4; i < 5; ++i)
                {
                    spectrum_.at(binInt + i).r += mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                    spectrum_.at(binInt + i).i += mag*sound->getBH((int)((binRem+i)*100) + 501)*sin(phase);
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
                        spectrum_.at(-1*(binInt + i)).r += mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                        spectrum_.at(-1*(binInt + i)).i += -mag*sound->getBH((int)((binRem+i)*100) + 501)*sin(phase);
                    }
                    // Real only at DC bin
                    else if ((binInt + i) == 0)
                    {
                        spectrum_.at(0).r += 2*mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                    }
                    // Regular
                    else
                    {
                        spectrum_.at(binInt + i).r += mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                        spectrum_.at(binInt + i).i += mag*sound->getBH((int)((binRem+i)*100) + 501)*sin(phase);
                    }
                }
            }
            // Wrap around the Nyquist bin
            else if (binLoc < (nyquistBin_ - 1) && binLoc >= (nyquistBin_-4))
            {
                for (int i = -4; i < 5; ++i)
                {
                    // Complex Conjugate wraps nyquist bin
                    if ((binInt + i) > nyquistBin_)
                    {
                        spectrum_.at((binInt + i) - nyquistBin_).r +=
                        mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                        spectrum_.at((binInt + i) - nyquistBin_).i +=
                        -mag*sound->getBH((int)((binRem+i)*100) + 501)*sin(phase);
                    }
                    // Real only at Nyquist
                    else if ((binInt + i) == nyquistBin_)
                    {
                        spectrum_.at(nyquistBin_).r += 2*mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                    }
                    // Regular
                    else
                    {
                        spectrum_.at(binInt + i).r += mag*sound->getBH((int)((binRem+i)*100) + 501)*cos(phase);
                        spectrum_.at(binInt + i).i += mag*sound->getBH((int)((binRem+i)*100) + 501)*sin(phase);
                    }
                }
            }
        }
    }
    
    float i = 1.0;
    env1_->apply(i);
    env1_->increment(hopSize_);
    
    // Conjugate for bins above the nyquist frequency
    for (int i = 1; i < nyquistBin_; ++i)
    {
        spectrum_.at(nyquistBin_ + i).r = spectrum_.at(nyquistBin_ - i).r;
        spectrum_.at(nyquistBin_ + i).i = -spectrum_.at(nyquistBin_ - i).i;
    }
    
    inverseFFT_->perform(spectrum_.data(), timeDomain_.data());
    
    // Apply synthesis window & shift
    for (int i = 0; i < frameSize_; ++i)
    {
        buffer(i) = timeDomain_.at((i + (frameSize_ / 2)) % frameSize_).r / frameSize_ * sound->getSynthWindow(i);
    }
    
    return true;
}

