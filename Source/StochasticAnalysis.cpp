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


void StochasticAnalysis::runAnalysis(SineModel::Ptr sineModel)
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
    
    // Temporary output for testing
    AudioBuffer<float> output(1, (sineModel->size() * hopSize) + frameSize);
    output.clear();
    float* outputSample = output.getWritePointer(0);
    
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
    int readPtr = hopSize;
    
    // Run audio processing loop
    while(frame != sineModel->end())
    {
        // Shift samples by a hop size
        for (int i = 0; i < hopSize * 3; i++)
        {
            inputSamples[i] = inputSamples[i + hopSize];
        }
        reader->read(&inputBuffer, hopSize * 3, hopSize, readPtr, true, false);
        readPtr += hopSize;
        
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
        
        // For testing output
        backwardFFT.perform(spectral.data(), outputTime.data());
        for (int i = 0; i < frameSize; ++i)
        {
            jassert((i + outPtr) < output.getNumSamples());
            outputSample[i + outPtr] += outputTime[(i + frameSize/2) % frameSize].r / frameSize * synthWindow_(i);
        }
        
        ++frame;
        outPtr += hopSize;
    }
    
    File outputFile("~/Development/output.wav");
    FileOutputStream* outputStream = new FileOutputStream(outputFile);
    AudioFormatWriter* write = WavAudioFormat().createWriterFor(outputStream, sampleRate_, 1, 16, StringPairArray(),0);
    
    write->writeFromAudioSampleBuffer(output, 0, output.getNumSamples());
    delete write;
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
