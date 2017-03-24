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
#include "AnalysisParameterManager.h"
#include "ParameterManager.h"
#include "FileLoader.h"

class SoundInterface
{
public:

    // Current state of this interface
    enum State
    {
        loadFileState,
        analysisState,
        runningAnalysisState,
        synthesisState
    };
    
    // Default Constructor
    SoundInterface(AnalysisParameterManager* a);
    
    // Default Deconstructor
    ~SoundInterface();
    
    // Create a new analysis object
    void buildAnalysis(File inputFile);
    
    // Run Analysis on current analysis
    void runAnalysis();
    
    // Sine Model Getter
    SineModel::ConstPtr getSineModel() const { return currentSineModel_; };
    
    // Get current state of sound
    State getState() const { return state_; };
    
    // Set current state of sound
    void setState(State newState) { state_ = newState; };
    
    // Get a parameter value from a parameter name
    float getParameterValue(String);

    // Get a pointer to the analysis parameters
    AnalysisParameterManager* getAnalysisParams() { return analysisParams_; };
    
    // Load audio from file and load into a new analysis object
    void loadFile();
    void loadFile(const String& fileName);
    
    // Pre-load check to see if a filename looks okay
    bool willAcceptFile(const String& fileName);
    
private:
    
    ScopedPointer<AnalysisMrs> analysis_;
    
    // Reference counted pointers to Sine Models for this interface
    ReferenceCountedArray<SineModel::Ptr> sineModels_;
    SineModel::Ptr currentSineModel_;
    
    
    State state_;
    
    FileLoader fileLoader_;
    
    // Parameters for the analysis phase
    AnalysisParameterManager* analysisParams_;
    
};


#endif  // SoundInterface_H_INCLUDED
