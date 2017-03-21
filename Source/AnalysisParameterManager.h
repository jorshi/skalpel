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
    
    int getFrameSize() const;
    int getHopSize() const;
    
    bool getRawValue(const String& name, float& value) const;

private:
    const float analysisWindowOffset = 9.0f;
    const float analysisHopSizeOffset = 6.0f;
    
};





#endif  // ANALYSISPARAMETERMANAGER_H_INCLUDED
