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
    AudioProcessorValueTreeState* parameters = params_.getParameters();
    Value val = parameters->getParameterAsValue(params_.getParamId("analysis_window"));
    
    // User defined parameters
    int hopSize = params_.getHopSize();
    int frameSize = params_.getFrameSize();
    float thresh;
    if (!params_.getRawValue("analysis_amp_thresh", thresh))
        thresh = -80.0;
    
    mrs_real sampleRate;
    
    // Complex vectors for frequency domain calculations
    std::vector<FFT::Complex> timeDomain(frameSize);
    std::vector<FFT::Complex> spectral(frameSize);
    
    // Forward FFT class
    FFT forwardFFT(std::log2(frameSize), false);
    
    // Create a Marsyas system
    MarSystemManager mng;
    ScopedPointer<MarSystem> network = mng.create("Series/analysis");
    
    network->addMarSystem(mng.create("SoundFileSource/input"));
    network->addMarSystem(mng.create("MixToMono/mono"));
    network->addMarSystem(mng.create("ShiftInput/shift"));
    network->addMarSystem(mng.create("Gain/gain"));

    const String fileName = audioFile_.getFullPathName();
    network->updControl("mrs_natural/inSamples", hopSize);
    network->updControl("SoundFileSource/input/mrs_string/filename", fileName.toStdString());
    network->updControl("ShiftInput/shift/mrs_natural/winSize", frameSize);
    
    // Get pointer to a frame of shifted samples from input
    MarControlPtr shifted = network->getControl("ShiftInput/shift/mrs_realvec/processedData");
    
    // Create and normalize a Hamming window
    mrs_realvec window(frameSize);
    SynthUtils::windowingFillHamming(window);
    window /= window.sum();
    
    // Run audio processing loop
    while(network->getControl("SoundFileSource/input/mrs_bool/hasData")->to_bool())
    {
        network->tick();
        
        // Update sample rate -- shouldn't change but we need to do it in the loop
        sampleRate = network->getControl("SoundFileSource/input/mrs_real/osrate")->to_real();
        
        // Shifted frame
        mrs_realvec input = shifted->to_realvec();
        
        // Store input as a complex number for FFT - Do Zero Phasing and apply window
        for (int i = 0; i < frameSize; ++i)
        {
            timeDomain.at(i).r = input((i + (int)(frameSize/2)) % frameSize) *
            window((i + (int)(frameSize/2)) % frameSize);
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data());
    }
}
