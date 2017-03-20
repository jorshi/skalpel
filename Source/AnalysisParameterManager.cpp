/*
  ==============================================================================

    AnalysisParameterManager.cpp
    Created: 19 Mar 2017 4:32:44am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisParameterManager.h"

AnalysisParameterManager::AnalysisParameterManager(int num, AudioProcessorValueTreeState* p) :
    ParameterManager(num, p)
{
    createParameters();
}


void AnalysisParameterManager::createParameters()
{
    String analysisWindowId("analysis_window_" + std::to_string(soundNum_));
    String analysisWindowName("analysis_window");
    parameters_->createAndAddParameter(analysisWindowId,
                                       analysisWindowName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 5.0f, 1.0f),
                                       2.0f,
                                       [](float value)
                                       {
                                           return std::to_string(1 << (int)(value + 9.0f));
                                       },
                                       [](const String& text)
                                       {
                                           return std::log2(std::stof(text.toStdString())) - 9.0f;
                                       });
    
    paramMap_.emplace(analysisWindowName, analysisWindowId);
}
