/*
  ==============================================================================

    SinusoidalSynthVoice.cpp
    Created: 17 Feb 2017 4:03:11pm
    Author:  Jordie Shier

  ==============================================================================
*/

#include "SinusoidalSynthVoice.h"


// Default Constructor
SinusoidalSynthVoice::SinusoidalSynthVoice(SoundInterfaceManager& s) :
    hopSize_(128), frameSize_(512), overlapIndex_(0), soundManger_(s)
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

//
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
        readPos_ = 0;
        writePos_ = 0;
        hopIndex_ = hopSize_;

        // Models producing sound -- we just want to hold onto these while a
        // sound is playing so that the model doesn't get switched out mid way through
        location_.resize(soundManger_.size());
        for (int i = 0; i < soundManger_.size(); i++)
        {
            if (soundManger_[i]->isActive())
            {
                activeModels_.insert(i, soundManger_[i]->getSineModel());
                activeNoiseModels_.insert(i, soundManger_[i]->getStochasticModel());
                params_.insert(i, soundManger_[i]->getSynthParams());
                location_.at(i) = 0.0f;
            }
        }

        previousElements_.resize(activeModels_.size());
        noteFreqScale_ = pow(2.0, (midiNoteNumber - sound->midiRootNote_)/12.0f);

        env1_ = soundManger_.getModulator("adsr_1");
        ADSR* adsr;
        if ((adsr = dynamic_cast<ADSR*>(env1_.get())))
        {
            adsr->triggerAttack();
        }
    }
    else
    {
        jassertfalse;
    }
}

void SinusoidalSynthVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff) {
        ADSR* adsr;
        if ((adsr = dynamic_cast<ADSR*>(env1_.get())))
        {
            adsr->triggerRelease();
        }
    }
    else
    {
        releaseOver();
    }
}

void SinusoidalSynthVoice::releaseOver()
{
    activeModels_.clear();
    params_.clear();
    previousElements_.clear();
    hopIndex_ = hopSize_;
    overlapIndex_ = 0;
    readPos_ = 0;
    buffer_.setval(0.0);
    clearCurrentNote();

    if (env1_ != nullptr)
    {
        env1_->setActive(false);
    }
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
                    env1_->setActive(false);
                }

                // Update write pointer and read pointer
                writePos_ = (writePos_ + hopSize_) % playingSound->getFrameSize();
                readPos_ = (readPos_ + hopSize_) % playingSound->getFrameSize();

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
    // No models to render or the amplitude envelope has completed
    if (activeModels_.size() < 1)
    {
        return false;
    }

    if (!env1_->isActive())
    {
        releaseOver();
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

    float parameterValue;

    // Tuning Parameters
    float octaveTuning;
    float semitoneTuning;
    float centTuning;
    float tuningRatio;

    // Time modulation
    float startTimeOffset;
    float playbackRate;

    // For continuous frequency scaling
    float freqScale;

    // For Frequency Stretching
    float stretchRatio;
    float stretchCenter;
    float stretchFactor;

    // For Frequency Shifting
    float freqShift;

    // Number of sinusoids to be included in playback
    float sineRatio;
    float frameEndOffset;

    // Sine gain parameter
    float sineGain;

    std::map<int, PrevElement>::iterator prev;

    for (int modelNum = activeModels_.size(); --modelNum >= 0;)
    {
        SineModel::ConstPtr model = activeModels_[modelNum];
        StochasticModel::ConstPtr noiseModel = activeNoiseModels_[modelNum];

        // Get number of frames in the model return if there aren't any
        if (model->size() < 1)
        {
            activeModels_.remove(modelNum);
            activeNoiseModels_.remove(modelNum);
            continue;
        }

        // Replace the float factor with a parameter, between 0 and 1
        startTimeOffset = params_.getUnchecked(modelNum)->getRawValue("start_offset", parameterValue) ?
            (parameterValue * model->size()) : 0.0f;

        playbackRate = params_.getUnchecked(modelNum)->getRawValue("playback_rate", parameterValue) ?
            parameterValue : 1.0f;

        // Get the frame closest to the requested time, float factor to be parameterized
        mrs_real requestedPos = (location_[modelNum] * model->getSampleRate()) / model->getFrameSize();
        int requestedFrame = std::round(requestedPos + startTimeOffset);

        // Update location
        location_[modelNum] += (hopSize_ / getSampleRate()) * playbackRate;

        // Out of frames from the model
        if (model->size() <= requestedFrame || noiseModel->size() <= requestedFrame)
        {
            activeModels_.remove(modelNum);
            activeNoiseModels_.remove(modelNum);
            continue;
        }

        // Amplitude envelope value to be applied at this frame
        env1_->apply(ampEnv);

        // Constant reference to the frame at this point
        const SineModel::SineFrame& frame = model->getFrame(requestedFrame);
        const StochasticModel::StochasticModelFrame& noiseFrame = noiseModel->getFrame(requestedFrame);

        // -- Tuning Parameters --
        octaveTuning = params_.getUnchecked(modelNum)->getRawValue("octave_tune", parameterValue) ?
            parameterValue : 0.0f;

        semitoneTuning = params_.getUnchecked(modelNum)->getRawValue("semitone_tune", parameterValue) ?
            parameterValue : 0.0f;

        centTuning = params_.getUnchecked(modelNum)->getRawValue("cent_tune", parameterValue) ?
            parameterValue : 0.0f;

        tuningRatio = ((octaveTuning * 1200.0f) + (semitoneTuning * 100.0f) + (centTuning)) / 1200.0f;
        tuningRatio = powf(2.0, tuningRatio);

        // --- Get transformation parameters outside of loop through frame --
        freqScale = params_.getUnchecked(modelNum)->getRawValue("frequency_scale", parameterValue) ?
            parameterValue : 1.0f;


        stretchCenter = params_.getUnchecked(modelNum)->getRawValue("stretch_center", parameterValue) ? parameterValue : 0.0f;

        stretchFactor = params_.getUnchecked(modelNum)->getRawValue("stretch_factor", parameterValue) ? parameterValue : 1.0f;

        freqShift = params_.getUnchecked(modelNum)->getRawValue("freq_shift", parameterValue) ?
            parameterValue : 0.0f;

        sineRatio = params_.getUnchecked(modelNum)->getRawValue("sine_ratio", parameterValue) ?
            parameterValue : 1.0f;

        sineGain = params_.getUnchecked(modelNum)->getRawValue("sine_gain", parameterValue) ?
            parameterValue : -6.0f;

        // Make sure the sine ratio is a number between 0 and 1
        if (isPositiveAndNotGreaterThan(sineRatio, 1.0f))
        {
            frameEndOffset = (int)((1.0f - sineRatio) * frame.size());
        }
        else
        {
            frameEndOffset = 0;
        }

        // ====================  Sinusoidal Modelling  ====================

        for (auto sine = frame.begin(); sine != (frame.end() - frameEndOffset); ++sine)
        {
            // Do frequency transformations here
            freq = sine->getFreq();

            // Frequency Scaling
            freq *= (noteFreqScale_ * freqScale * tuningRatio);

            // Perform stretching
            stretchRatio = freq / stretchCenter;
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
            freq += freqShift;

            binLoc =  (freq / getSampleRate()) * frameSize_;
            binInt = std::round(binLoc);
            binRem = binInt - binLoc;

            // Convert the decibels back to magnitude. Gain should be parameterized.
            mag = pow(10, (sine->getAmp() + sineGain)/20) * ampEnv;

            // Propagate phase
            if ((prev = previousElements_.at(modelNum).find(sine->getTrack())) ==  previousElements_.at(modelNum).end())
            {
                phase = sine->getPhase();
                previousElements_.at(modelNum).emplace(sine->getTrack(), PrevElement(freq, sine->getPhase()));
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

        // ====================  Stochastic Modelling  ====================

        // Do a simple linear interpolation resampling ot get correct number of samples
        float factor = std::floor(frameSize_ / noiseFrame.size());
        float noiseLoc, noiseMag, noisePhase;
        int prevBin, nextBin;
        for (int i = 1; i < nyquistBin_; i++)
        {
            noiseLoc = i / factor;
            prevBin = std::floor(noiseLoc);
            nextBin = std::ceil(noiseLoc);
            noiseLoc = noiseLoc - prevBin;

            // Do linear interpolation and convert from dB to linear scale magnitude
            noiseMag = noiseFrame[prevBin] + noiseLoc * (noiseFrame[nextBin] - noiseFrame[prevBin]);

            jassert(noiseMag < 0.0);
            noiseMag = powf(10, noiseMag/20.0) * ampEnv;

            // Random phase for shaped noise
            noisePhase = random_.nextFloat() * 2 * float_Pi;

            // Create noise spectrum
            spectrum_.at(i).r += noiseMag*cos(noisePhase);
            spectrum_.at(i).i += noiseMag*sin(noisePhase);
        }
    }

    // TODO -- what's this about?
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

