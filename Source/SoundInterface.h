/*
  ==============================================================================

    SoundInterface.h
    Created: 18 Mar 2017 12:28:33pm
    Author:  Jordie Shier 
 
    This should store information related to an sound -- analysis and synthesis

  ==============================================================================
*/

#ifndef SoundInterface_H_INCLUDED
#define SoundInterface_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "AnalysisMrs.h"
#include "SineElement.h"

class SoundInterface
{
public:
    
    // Default Constructor
    SoundInterface();
    
    // Default Deconstructor
    ~SoundInterface();
    
    // Create a new analysis object
    void buildAnalysis(File inputFile);
    
    // Run Analysis on current analysis
    void runAnalysis();
    
    const SineModel& getSineModel() const { return *sineModel_; };
    
private:
    
    ScopedPointer<AnalysisMrs> analysis_;
    ScopedPointer<SineModel> sineModel_;
    
};


#endif  // SoundInterface_H_INCLUDED
