/*
  ==============================================================================

    AnalysisMrs.h
    Created: 10 Feb 2017 10:05:14am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ANALYSISMRS_H_INCLUDED
#define ANALYSISMRS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "FileLoader.h"
#include "marsyas/system/MarSystemManager.h"
#include "SynthesisUtils.h"
#include "AnalysisParameterManager.h"
#include "SineElement.h"
#include <vector>
#include <algorithm>


class AnalysisMrs
{
public:
    
    // Defualt Constructor
    AnalysisMrs(AnalysisParameterManager& params);
    
    // Constructor with audio file
    AnalysisMrs(File input, AnalysisParameterManager& params);
    
    // Default Deconstructor
    ~AnalysisMrs();
    
    // Run analysis on file, return a pointer to the created model object
    // !! Not responsible for cleaning up the object !!
    SineModel::Ptr runAnalysis();
    
private:
    void peakDetection(SineModel::Ptr, String, AudioFormatReader* reader);
    void sineTracking(SineModel::Ptr);
    void cleanModel(SineModel::Ptr);
    
    File audioFile;
    FileLoader fileLoader_;
    
    // Paramters
    AnalysisParameterManager& params_;
};



#endif  // ANALYSISMRS_H_INCLUDED
