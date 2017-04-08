/*
  ==============================================================================

    StochasticAnalysis.cpp
    Created: 7 Apr 2017 6:05:42am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "StochasticAnalysis.h"

using namespace Marsyas;


StochasticAnalysis::StochasticAnalysis(File input, AnalysisParameterManager& p) :
    audioFile_(input), params_(p)
{
}


StochasticModel::Ptr StochasticAnalysis::runAnalysis(SineModel::Ptr sineModel)
{
    StochasticModel::Ptr model;
    
    AudioBuffer<float> residual;
    residual.clear();
    residualSignal(residual, sineModel);
    model = stochasticModelling(residual, 0, 128, sineModel->size());
    
    // Make sure these are the same size
    jassert(sineModel->size() == model->size());
    
    return model;
}


void StochasticAnalysis::residualSignal(AudioBuffer<float> &residual, SineModel::Ptr sineModel)
{
    // Audio file reader
    ScopedPointer<AudioFormatReader> reader = fileLoader_.getAudioReader(audioFile_);
    sampleRate_ = reader->sampleRate;
    
    // Maybe not any definable parameters?
    //AudioProcessorValueTreeState* parameters = params_.getParameters();
    int hopSize = 128;
    int frameSize = 512;
    
    synthWindow_.create(frameSize);
    SynthUtils::createSynthesisWindow(synthWindow_, hopSize);
    
    // For now this needs to be the same, can probably be definable later if we want
    jassert(hopSize == sineModel->getHopSize());
    
    // Create a buffer for storing the residual signal
    residual.setSize(1, (sineModel->size() * hopSize) + frameSize);
    float* residualSample = residual.getWritePointer(0);
    
    // Audio buffer for store input from the loaded audio file
    AudioBuffer<float> inputBuffer(1, frameSize);
    
    // Complex vectors for frequency domain calculations
    std::vector<FFT::Complex> timeDomain(frameSize);
    std::vector<FFT::Complex> spectral(frameSize);
    std::vector<FFT::Complex> sineSpectral(frameSize);
    std::vector<FFT::Complex> outputTime(frameSize);
    
    // Forward FFT class
    FFT forwardFFT(std::log2(frameSize), false);
    FFT backwardFFT(std::log2(frameSize), true);
    
    // Create and normalize a Hamming window
    mrs_realvec window(frameSize);
    SynthUtils::windowingFillBlackmanHarris(window);
    window /= window.sum();
    
    // Model frame iterator
    auto frame = sineModel->begin();
    
    int outPtr = 0;
    
    float* inputSamples = inputBuffer.getWritePointer(0);
    inputBuffer.clear();
    
    // First half of frame is padded with zeros
    reader->read(&inputBuffer, frameSize/2, frameSize/2, 0, true, false);
    int readPtr = frameSize/2;
    
    // Run audio processing loop
    while(frame != sineModel->end())
    {
        // Store input as a complex number for FFT - Do Zero Phasing and apply window
        for (int i = 0; i < frameSize; ++i)
        {
            timeDomain.at(i).r = inputSamples[(i + (int)(frameSize/2)) % frameSize] *
            window((i + (int)(frameSize/2)) % frameSize);
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data());
        
        // Synthesize this frame, but zero it out first
        std::for_each(sineSpectral.begin(), sineSpectral.end(), [](FFT::Complex& c) {
            c.r = 0;
            c.i = 0;
        });
        
        // Generate the sinusoidal spectrum
        synthesizeFrame(*frame, sineSpectral.data(), sineSpectral.size());
        
        // Subtract the sinusoidal spectrum from the original spectrum
        for (int i = 0; i < frameSize; i++)
        {
            spectral[i].r -= sineSpectral[i].r;
            spectral[i].i -= sineSpectral[i].i;
        }
        
        // Create a residual signal
        backwardFFT.perform(spectral.data(), outputTime.data());
        for (int i = 0; i < frameSize; ++i)
        {
            jassert((i + outPtr) < residual.getNumSamples());
            residualSample[i + outPtr] += outputTime[(i + frameSize/2) % frameSize].r / frameSize * synthWindow_(i);
        }
        
        // Shift samples by a hop size
        for (int i = hopSize; i < frameSize; i++)
        {
            inputSamples[i - hopSize] = inputSamples[i];
        }
        reader->read(&inputBuffer, frameSize - hopSize, hopSize, readPtr, true, false);
        readPtr += hopSize;
        
        ++frame;
        outPtr += hopSize;
    }
}


void StochasticAnalysis::synthesizeFrame(SineModel::SineFrame &frame, FFT::Complex* spectrum, int length)
{
    float freq, phase, mag;
    float binLoc, binRem;
    int binInt;
    int nyquistBin = length / 2;
    
    // Create the spectral signal
    for (auto sine = frame.begin(); sine != frame.end(); ++sine)
    {
        // Do frequency transformations here
        freq = sine->getFreq();
        phase = sine->getPhase();

        binLoc =  (freq / sampleRate_) * length;
        binInt = std::round(binLoc);
        binRem = binInt - binLoc;
        
        // Convert the decibels back to magnitude. Gain should be parameterized.
        mag = pow(10, sine->getAmp()/20);

        
        // Going to make a 9 bin wide Blackman Harris window
        if (binLoc >= 5 && binLoc < nyquistBin-4)
        {
            for (int i = -4; i < 5; ++i)
            {
                spectrum[binInt + i].r += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                spectrum[binInt + i].i += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase);
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
                    spectrum[-1*(binInt + i)].r += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                    spectrum[-1*(binInt + i)].i += -mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase);
                }
                // Real only at DC bin
                else if ((binInt + i) == 0)
                {
                    spectrum[0].r += 2*mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                }
                // Regular
                else
                {
                    spectrum[binInt + i].r += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                    spectrum[binInt + i].i += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase);
                }
            }
        }
        // Wrap around the Nyquist bin
        else if (binLoc < (nyquistBin - 1) && binLoc >= (nyquistBin-4))
        {
            for (int i = -4; i < 5; ++i)
            {
                // Complex Conjugate wraps nyquist bin
                if ((binInt + i) > nyquistBin)
                {
                    spectrum[(binInt + i) - nyquistBin].r +=
                        mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                    spectrum[(binInt + i) - nyquistBin].i +=
                        -mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase);
                }
                // Real only at Nyquist
                else if ((binInt + i) == nyquistBin)
                {
                    spectrum[nyquistBin].r += 2*mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                }
                // Regular
                else
                {
                    spectrum[binInt + i].r += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase);
                    spectrum[binInt + i].i += mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase);
                }
            }
        }
    }
    
    // Conjugate for bins above the nyquist frequency
    for (int i = 1; i < nyquistBin; ++i)
    {
        spectrum[nyquistBin + i].r = spectrum[nyquistBin - i].r;
        spectrum[nyquistBin + i].i = -spectrum[nyquistBin - i].i;
    }
}


StochasticModel* StochasticAnalysis::stochasticModelling(AudioBuffer<float>& residualBuffer, int startSample,
                                                         int hopSize, int frames)
{
    const float* sample = residualBuffer.getReadPointer(0);
    int readPtr = startSample;
    int factor = hopSize;
    
    // New stochastic model
    StochasticModel* stochasticModel = new StochasticModel(factor);
    stochasticModel->setHopSize(hopSize);
    stochasticModel->setSampleRate(sampleRate_);
    
    // Initial move to frequency domain
    std::vector<FFT::Complex> time(hopSize*2);
    std::vector<FFT::Complex> spectral(hopSize*2);
    FFT forward(std::log2(hopSize*2), false);
    
    // For resampling
    std::vector<FFT::Complex> resampleSpectral(hopSize);
    
    // Normalized Hamming window
    mrs_realvec window;
    window.create(hopSize*2);
    SynthUtils::windowingFillHamming(window);
    window /= window.sum();
    
    int frameCount = 0;
    while (readPtr + (hopSize*2) < residualBuffer.getNumSamples() && frameCount < frames)
    {
        // Window residual signal
        for (int i = 0; i < hopSize * 2; i++)
        {
            time[i].r = sample[readPtr + i] * window(i);
            jassert(std::abs(time[i].r) <= 1.0);
        }
        
        // Move to frequency domain
        forward.perform(time.data(), spectral.data());
        
        // Calculate magnitude spectrum
        for (int i = 0; i < hopSize; i++)
        {
            resampleSpectral[i].r = sqrt(spectral[i].r * spectral[i].r + spectral[i].i * spectral[i].i);
            resampleSpectral[i].r = std::max(20*log10(resampleSpectral[i].r), -200.0);
            resampleSpectral[i].i = 0.0;
            
            // Make sure levels are correct
            jassert(resampleSpectral[i].r < 0.0);
        }
        
        // Add frame to the stochastic model
        stochasticModel->addFrame(resampleSpectral);
        
        readPtr += hopSize;
        frameCount++;
    }
    
    return stochasticModel;
}
