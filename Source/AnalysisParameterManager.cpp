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

    // Analysis Window Size
    String analysisWindowId("analysis_window_" + std::to_string(soundNum_));
    String analysisWindowName("analysis_window");
    paramMap_.emplace(analysisWindowName, analysisWindowId);
    parameters_->createAndAddParameter(analysisWindowId,
                                       analysisWindowName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 5.0f, 1.0f),
                                       2.0f,
                                       [=](float value)
                                       {
                                           return std::to_string(1 << (int)(value + analysisWindowOffset));
                                       },
                                       [=](const String& text)
                                       {
                                           return std::log2(std::stof(text.toStdString())) - analysisWindowOffset;
                                       });


    // Analysis Hop Size
    String analysisHopId("analysis_hop_" + std::to_string(soundNum_));
    String analysisHopName("analysis_hop");
    paramMap_.emplace(analysisHopName, analysisHopId);
    parameters_->createAndAddParameter(analysisHopId,
                                       analysisHopName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 3.0f, 1.0f),
                                       1.0f,
                                       [=](float value)
                                       {
                                           return std::to_string(1 << (int)(value + analysisHopSizeOffset));
                                       },
                                       [=](const String& text)
                                       {
                                           return std::log2(std::stof(text.toStdString())) - analysisHopSizeOffset;
                                       });

    // Analysis Amplitude Threshold
    String analysisAmpThreshId("analysis_amp_thresh_" + std::to_string(soundNum_));
    String analysisAmpThreshName("analysis_amp_thresh");
    paramMap_.emplace(analysisAmpThreshName, analysisAmpThreshId);
    parameters_->createAndAddParameter(analysisAmpThreshId,
                                       analysisAmpThreshName,
                                       "dB",
                                       NormalisableRange<float>(-120.0f, 0.0f, 1.0f),
                                       -80.0f, nullptr, nullptr);

    // Analysis Frequency Deviation Offset
    String analysisFreqOffsetId("analysis_freq_offset_" + std::to_string(soundNum_));
    String analysisFreqOffsetName("analysis_freq_offset");
    paramMap_.emplace(analysisFreqOffsetName, analysisFreqOffsetId);
    parameters_->createAndAddParameter(analysisFreqOffsetId,
                                       analysisFreqOffsetName,
                                       "Hz",
                                       NormalisableRange<float>(0.0f, 100.0f, 1.0f, 0.75f),
                                       20.0f, nullptr, nullptr);

    // Analysis Frequency Deviation Slope
    String analysisFreqSlopeId("analysis_freq_slope_" + std::to_string(soundNum_));
    String analysisFreqSlopeName("analysis_freq_slope");
    paramMap_.emplace(analysisFreqSlopeName, analysisFreqSlopeId);
    parameters_->createAndAddParameter(analysisFreqSlopeId,
                                       analysisFreqSlopeName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 0.01f, 0.0001f, 0.75f),
                                       0.001f, nullptr, nullptr);

    // Analysis Sine Tracking Duration
    String analysisDurationId("analysis_duration_" + std::to_string(soundNum_));
    String analysisDurationName("analysis_duration");
    paramMap_.emplace(analysisDurationName, analysisDurationId);
    parameters_->createAndAddParameter(analysisDurationId,
                                       analysisDurationName,
                                       "ms",
                                       NormalisableRange<float>(0.0f, 5000.0f, 0.1f, 0.25f),
                                       20.0f, nullptr, nullptr);

    String analysisSineCountId("analysis_sines_" + std::to_string(soundNum_));
    String analysisSineCountName("analysis_sines");
    paramMap_.emplace(analysisSineCountName, analysisSineCountId);
    parameters_->createAndAddParameter(analysisSineCountId,
                                       analysisSineCountName,
                                       String(),
                                       NormalisableRange<float>(1.0f, 200.0f, 1.0f),
                                       150.0f, nullptr, nullptr);
}


int AnalysisParameterManager::getFrameSize() const
{
    float value = *parameters_->getRawParameterValue(getParamId("analysis_window"));
    return 1 << (int)(value + analysisWindowOffset);
}


int AnalysisParameterManager::getHopSize() const
{
    float value = *parameters_->getRawParameterValue(getParamId("analysis_hop"));
    return 1 << (int)(value + analysisHopSizeOffset);
}

bool AnalysisParameterManager::getRawValue(const juce::String &name, float& value) const
{
    String paramId;
    if ((paramId = getParamId(name)) != "")
    {
        value = *parameters_->getRawParameterValue(paramId);
        return true;
    }

    return false;
}
