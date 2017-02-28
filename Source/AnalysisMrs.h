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
    AnalysisMrs();
    ~AnalysisMrs();
    
    void newAnalysis();
    
    const SineModel& getAnalysisModel() const { return _analysisModel; };
    
private:
    void peakDetection(SineModel&, String);
    void sineTracking(SineModel&);
    void cleanModel(SineModel&);
    
    SineModel _analysisModel;
};



#endif  // ANALYSISMRS_H_INCLUDED
