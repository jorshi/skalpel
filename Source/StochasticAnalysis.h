/*
  ==============================================================================

    StochasticAnalysis.h
    Created: 7 Apr 2017 6:05:42am
    Author:  Jordie Shier

  ==============================================================================
*/

#ifndef STOCHASTICANALYSIS_H_INCLUDED
#define STOCHASTICANALYSIS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "marsyas/system/MarSystemManager.h"
#include <algorithm>
#include "FileLoader.h"
#include "SineElement.h"
#include "SynthesisUtils.h"
#include "AnalysisParameterManager.h"
#include "StochasticModel.h"

class StochasticAnalysis
{
public:

    // Constructor with input file fo
    StochasticAnalysis(File input, AnalysisParameterManager& p);

    // Default Deconstructor
    ~StochasticAnalysis() {};

    // Run stochastic analysis
    StochasticModel::Ptr runAnalysis(SineModel::Ptr sineModel);

private:

    // Synthesize a frame of the sine model to subtract from the spectrum
    void synthesizeFrame(SineModel::SineFrame& frame, FFT::Complex* spectrum, int length);

    // Calculate a stochastic model for a frame
    StochasticModel* stochasticModelling(AudioBuffer<float>& residualSignal, int startSample, int hopSize, int frames);

    // Create residual signal
    void residualSignal(AudioBuffer<float>& residualSignal, SineModel::Ptr sineModel);

    File audioFile_;
    AnalysisParameterManager& params_;

    FileLoader fileLoader_;

    mrs_real sampleRate_;
    mrs_realvec synthWindow_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StochasticAnalysis);
};




#endif  // STOCHASTICANALYSIS_H_INCLUDED
