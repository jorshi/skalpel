/*
  ==============================================================================

    SynthesisParameterManager.cpp
    Created: 30 Mar 2017 12:32:12am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SynthesisParameterManager.h"


SynthesisParameterManager::SynthesisParameterManager(int num, AudioProcessorValueTreeState* p) :
    ParameterManager(num, p)
{
    createParameters();
}


void SynthesisParameterManager::createParameters()
{
    String octaveTuneId("octave_tune_" + std::to_string(soundNum_));
    String octaveTuneName("octave_tune");
    paramMap_.emplace(octaveTuneName, octaveTuneId);
    parameters_->createAndAddParameter(octaveTuneId,
                                       octaveTuneName,
                                       String(),
                                       NormalisableRange<float>(-2.0f, 2.0f, 1.0f),
                                       0.0f,
                                       [](float value) { return std::to_string((int)value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String semitoneTuneId("semitone_tune_" + std::to_string(soundNum_));
    String semitoneTuneName("semitone_tune");
    paramMap_.emplace(semitoneTuneName, semitoneTuneId);
    parameters_->createAndAddParameter(semitoneTuneId,
                                       semitoneTuneName,
                                       "semitones",
                                       NormalisableRange<float>(-12.0f, 12.0f, 1.0f),
                                       0.0f,
                                       [](float value) { return std::to_string((int)value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String centTuneId("cent_tune_" + std::to_string(soundNum_));
    String centTuneName("cent_tune");
    paramMap_.emplace(centTuneName, centTuneId);
    parameters_->createAndAddParameter(centTuneId,
                                       centTuneName,
                                       "cents",
                                       NormalisableRange<float>(-50.0f, 50.0f, 1.0f),
                                       0.0f,
                                       [](float value) { return std::to_string((int)value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String freqScaleId("frequency_scale_" + std::to_string(soundNum_));
    String freqScaleName("frequency_scale");
    paramMap_.emplace(freqScaleName, freqScaleId);
    parameters_->createAndAddParameter(freqScaleId,
                                       freqScaleName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 4.0f),
                                       1.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });

    
    String stretchCenterId("stretch_center_" + std::to_string(soundNum_));
    String stretchCenterName("stretch_center");
    paramMap_.emplace(stretchCenterName, stretchCenterId);
    parameters_->createAndAddParameter(stretchCenterId,
                                       stretchCenterName,
                                       "Hz",
                                       NormalisableRange<float>(0.0f, 10000.0f, 0.0f, 0.75f),
                                       100.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String stretchFactorId("stretch_factor_" + std::to_string(soundNum_));
    String stretchFactorName("stretch_factor");
    paramMap_.emplace(stretchFactorName, stretchFactorId);
    parameters_->createAndAddParameter(stretchFactorId,
                                       stretchFactorName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 1.0f),
                                       0.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String freqShiftId("freq_shift_" + std::to_string(soundNum_));
    String freqShiftName("freq_shift");
    paramMap_.emplace(freqShiftName, freqShiftId);
    parameters_->createAndAddParameter(freqShiftId,
                                       freqShiftName,
                                       "Hz",
                                       NormalisableRange<float>(0.0f, 1000.0f, 0.0f, 0.75f),
                                       0.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String startOffsetId("start_offset_" + std::to_string(soundNum_));
    String startOffsetName("start_offset");
    paramMap_.emplace(startOffsetName, startOffsetId);
    parameters_->createAndAddParameter(startOffsetId,
                                       startOffsetName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 1.0f),
                                       0.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String playbackRateId("playback_rate_" + std::to_string(soundNum_));
    String playbackRateName("playback_rate");
    paramMap_.emplace(playbackRateName, playbackRateId);
    parameters_->createAndAddParameter(playbackRateId,
                                       playbackRateName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 4.0f),
                                       1.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
    
    String sineRatioId("sine_ratio_" + std::to_string(soundNum_));
    String sineRatioName("sine_ratio");
    paramMap_.emplace(sineRatioName, sineRatioId);
    parameters_->createAndAddParameter(sineRatioId,
                                       sineRatioName,
                                       String(),
                                       NormalisableRange<float>(0.0f, 1.0f),
                                       1.0f,
                                       [](float value) { return std::to_string(value); },
                                       [](const String& text) { return std::stof(text.toStdString()); });
}


bool SynthesisParameterManager::getRawValue(const juce::String &name, float& value) const
{
    String paramId;
    if ((paramId = getParamId(name)) != "")
    {
        value = *parameters_->getRawParameterValue(paramId);
        return true;
    }
    
    return false;
}
