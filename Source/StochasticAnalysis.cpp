/*
  ==============================================================================

    StochasticAnalysis.cpp
    Created: 7 Apr 2017 6:05:42am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "StochasticAnalysis.h"


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
    
    synthWindow_.resize(frameSize);
    SynthUtils::createSynthesisWindow(synthWindow_, hopSize);
    
    // For now this needs to be the same, can probably be definable later if we want
    jassert(hopSize == sineModel->getHopSize());
    
    // Create a buffer for storing the residual signal
    residual.setSize(1, (sineModel->size() * hopSize) + frameSize);
    float* residualSample = residual.getWritePointer(0);
    
    // Audio buffer for store input from the loaded audio file
    AudioBuffer<float> inputBuffer(1, frameSize);
    
    // Complex vectors for frequency domain calculations
    std::vector<dsp::Complex<float>> timeDomain(frameSize);
    std::vector<dsp::Complex<float>> spectral(frameSize);
    std::vector<dsp::Complex<float>> sineSpectral(frameSize);
    std::vector<dsp::Complex<float>> outputTime(frameSize);
    
    // Forward FFT class
    dsp::FFT forwardFFT(std::log2(frameSize));
    dsp::FFT backwardFFT(std::log2(frameSize));
    
    // Create and normalize a Hamming window
    std::vector<double> window(frameSize);
    SynthUtils::windowingFillBlackmanHarris(window);
    auto sum = std::accumulate(window.begin(), window.end(), 0.0);
    std::for_each(window.begin(), window.end(), [sum](auto &c){ c /= sum; });
    
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
            auto new_real = inputSamples[(i + (int)(frameSize/2)) % frameSize] * window[(i + (int)(frameSize/2)) % frameSize];
            timeDomain.at(i).real(new_real);
            
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data(), false);
        
        // Synthesize this frame, but zero it out first
        std::for_each(sineSpectral.begin(), sineSpectral.end(), [](auto &c) {
            c.real(0.0);
            c.imag(0.0);
        });
        
        // Generate the sinusoidal spectrum
        synthesizeFrame(*frame, sineSpectral.data(), (int)sineSpectral.size());
        
        // Subtract the sinusoidal spectrum from the original spectrum
        for (int i = 0; i < frameSize; i++)
        {
            auto real = spectral[i].real();
            auto imag = spectral[i].imag();
            spectral[i].real(real - sineSpectral[i].real());
            spectral[i].imag(imag - sineSpectral[i].imag());
        }
        
        // Create a residual signal
        backwardFFT.perform(spectral.data(), outputTime.data(), true);
        for (int i = 0; i < frameSize; ++i)
        {
            jassert((i + outPtr) < residual.getNumSamples());
            residualSample[i + outPtr] += outputTime[(i + frameSize/2) % frameSize].real() / frameSize * synthWindow_[i];
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


void StochasticAnalysis::synthesizeFrame(SineModel::SineFrame &frame,
                                         dsp::Complex<float>* spectrum,
                                         int length)
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
                auto bin = binInt + i;
                auto real = spectrum[binInt + i].real();
                auto imag = spectrum[binInt + i].imag();
                spectrum[bin].real(real + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                spectrum[bin].imag(imag + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase));
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
                    auto bin = -1*(binInt + i);
                    auto real = spectrum[bin].real();
                    auto imag = spectrum[bin].imag();
                    spectrum[bin].real(real + mag * SynthUtils::BHCONST[(int)((binRem + i) * 100) + 501] * cos(phase));
                    spectrum[bin].imag(imag + -mag * SynthUtils::BHCONST[(int)((binRem+i)*100) + 501] * sin(phase));
                }
                // Real only at DC bin
                else if ((binInt + i) == 0)
                {
                    auto real = spectrum[0].real();
                    spectrum[0].real(real + 2*mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                }
                // Regular
                else
                {
                    auto bin = binInt + i;
                    auto real = spectrum[bin].real();
                    auto imag = spectrum[bin].imag();
                    spectrum[bin].real(real + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                    spectrum[bin].imag(imag + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase));
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
                    auto bin = (binInt + i) - nyquistBin;
                    auto real = spectrum[bin].real();
                    auto imag = spectrum[bin].imag();
                    spectrum[bin].real(real + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                    spectrum[bin].imag(imag + -mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase));
                }
                // Real only at Nyquist
                else if ((binInt + i) == nyquistBin)
                {
                    auto real = spectrum[nyquistBin].real();
                    spectrum[nyquistBin].real(real + 2*mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                }
                // Regular
                else
                {
                    auto bin = binInt + i;
                    auto real = spectrum[bin].real();
                    auto imag = spectrum[bin].imag();
                    spectrum[bin].real(real + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*cos(phase));
                    spectrum[bin].imag(imag + mag*SynthUtils::BHCONST[(int)((binRem+i)*100) + 501]*sin(phase));
                }
            }
        }
    }
    
    // Conjugate for bins above the nyquist frequency
    for (int i = 1; i < nyquistBin; ++i)
    {
        spectrum[nyquistBin + i].real(spectrum[nyquistBin - i].real());
        spectrum[nyquistBin + i].imag(-spectrum[nyquistBin - i].imag());
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
    std::vector<dsp::Complex<float>> time(hopSize*2);
    std::vector<dsp::Complex<float>> spectral(hopSize*2);
    dsp::FFT forward(std::log2(hopSize*2));
    
    // For resampling
    std::vector<dsp::Complex<float>> resampleSpectral(hopSize);
    
    // Normalized Hamming window
    std::vector<double> window;
    window.resize(hopSize*2);
    SynthUtils::windowingFillHamming(window);
    auto sum = std::accumulate(window.begin(), window.end(), 0.0);
    std::for_each(window.begin(), window.end(), [sum](auto& c){ c /= sum; });
    
    int frameCount = 0;
    while (readPtr + (hopSize*2) < residualBuffer.getNumSamples() && frameCount < frames)
    {
        // Window residual signal
        for (int i = 0; i < hopSize * 2; i++)
        {
            time[i].real(sample[readPtr + i] * window[i]);
            jassert(std::abs(time[i].real()) <= 1.0);
        }
        
        // Move to frequency domain
        forward.perform(time.data(), spectral.data(), false);
        
        // Calculate magnitude spectrum
        for (int i = 0; i < hopSize; i++)
        {
            resampleSpectral[i].real(std::sqrt(spectral[i].real() * spectral[i].real() + spectral[i].imag() * spectral[i].imag()));
            resampleSpectral[i].real(std::max((double)20.0*log10(resampleSpectral[i].real()), -200.0));
            resampleSpectral[i].imag(0.0);
            
            // Make sure levels are correct
            jassert(resampleSpectral[i].real() < 0.0);
        }
        
        // Add frame to the stochastic model
        stochasticModel->addFrame(resampleSpectral);
        
        readPtr += hopSize;
        frameCount++;
    }
    
    return stochasticModel;
}
