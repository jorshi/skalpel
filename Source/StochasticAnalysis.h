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
#include "SineElement.h"
#include "SynthesisUtils.h"
#include "AnalysisParameterManager.h"

class StochasticAnalysis
{
public:
    
    // Constructor with input file fo
    StochasticAnalysis(File input, AnalysisParameterManager& p);
    
    // Default Deconstructor
    ~StochasticAnalysis() {};
    
    // Run stochastic analysis
    void runAnalysis(SineModel::Ptr sineModel);
    
private:
    
    // Synthesize a frame of the sine model to subtract from the spectrum
    void synthesizeFrame(SineModel::SineFrame& frame, FFT::Complex* spectrum, int length);
  
    File audioFile_;
    AnalysisParameterManager& params_;
    
    mrs_real sampleRate_;
    mrs_realvec synthWindow_;
};




#endif  // STOCHASTICANALYSIS_H_INCLUDED
