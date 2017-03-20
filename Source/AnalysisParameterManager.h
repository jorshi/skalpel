/*
  ==============================================================================

    AnalysisParameterManager.h
    Created: 19 Mar 2017 4:32:44am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ANALYSISPARAMETERMANAGER_H_INCLUDED
#define ANALYSISPARAMETERMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ParameterManager.h"

class AnalysisParameterManager : public ParameterManager
{
public:
    
    AnalysisParameterManager(int num, AudioProcessorValueTreeState* p);
    
    ~AnalysisParameterManager() override {};
    
    void createParameters() override;
};





#endif  // ANALYSISPARAMETERMANAGER_H_INCLUDED
