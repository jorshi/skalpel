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
#include "marsyas/system/MarSystemManager.h"
#include "SynthesisUtils.h"
#include "SineElement.h"
#include <vector>
#include <algorithm>


class AnalysisMrs
{
public:
    
    // Defualt Constructor
    AnalysisMrs();
    
    // Constructor with audio file
    AnalysisMrs(File);
    
    // Default Deconstructor
    ~AnalysisMrs();
    
    // Asks for an audio file and runs analysis on the file, storing the results
    // in the member analysis model
    void newAnalysis();
    
    // Run analysis on file, return a pointer to the created model object
    // !! Not responsible for cleaning up the object !!
    SineModel* runAnalysis();
    
    const SineModel& getAnalysisModel() const { return _analysisModel; };
    
private:
    void peakDetection(SineModel&, String);
    void sineTracking(SineModel&);
    void cleanModel(SineModel&);
    
    SineModel _analysisModel;
    
    File audioFile;
};



#endif  // ANALYSISMRS_H_INCLUDED
