/*
  ==============================================================================

    AnalysisComponent.cpp
    Created: 19 Mar 2017 12:36:26am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "AnalysisComponent.h"

//==============================================================================
AnalysisComponent::AnalysisComponent(Button::Listener* parent, AnalysisParameterManager* p) : analysisParams(p)
{
    analysisButton.setButtonText("Run Analysis");
    analysisButton.setComponentID("run_analysis");
    analysisButton.addListener(parent);
    addAndMakeVisible(&analysisButton);
    
    // FFT Size
    addAndMakeVisible(windowSize);
    addComboBoxOptions(&windowSize, "analysis_window");
    windowAttachment = new ComboBoxAttachment(*analysisParams->getParameters(),
                                              analysisParams->getParamId("analysis_window"),
                                              windowSize);
    
    addAndMakeVisible(windowLabel);
    windowLabel.setText("FFT Size", dontSendNotification);
    windowLabel.attachToComponent(&windowSize, false);
    
//    addAndMakeVisible(hopSize);
//    addComboBoxOptions(&hopSize, "analysis_hop");
//    hopAttachment = new  ComboBoxAttachment(*analysisParams->getParameters(),
//                                            analysisParams->getParamId("analysis_hop"),
//                                            hopSize);
//
    
    // Amplitude Threshold
    addAndMakeVisible(amplitudeTreshold);
    amplitudeTreshold.setTextValueSuffix(" dB");
    amplitudeAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                               analysisParams->getParamId("analysis_amp_thresh"),
                                               amplitudeTreshold);
    
    addAndMakeVisible(amplitudeLabel);
    amplitudeLabel.setText("Amplitude Threshold", dontSendNotification);
    amplitudeLabel.attachToComponent(&amplitudeTreshold, false);
    
    
    // Min Sine Duration
    addAndMakeVisible(duration);
    duration.setTextValueSuffix(" ms");
    durationAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                              analysisParams->getParamId("analysis_duration"),
                                              duration);
    
    addAndMakeVisible(durationLabel);
    durationLabel.setText("Minimum Sine Duration", dontSendNotification);
    durationLabel.attachToComponent(&duration, false);
    
    // Frequency Offset 
    addAndMakeVisible(freqOffset);
    freqOffset.setTextValueSuffix(" Hz");
    freqOffsetAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                               analysisParams->getParamId("analysis_freq_offset"),
                                               freqOffset);
    
    addAndMakeVisible(freqOffsetLabel);
    freqOffsetLabel.setText("Minimum Frequency Deviation Offset", dontSendNotification);
    freqOffsetLabel.attachToComponent(&freqOffset, false);
    
//    addAndMakeVisible(freqSlope);
//    freqSlopeAttachment = new SliderAttachment(*analysisParams->getParameters(),
//                                               analysisParams->getParamId("analysis_freq_slope"),
//                                               freqSlope);
    
    addAndMakeVisible(sines);
    sinesAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                           analysisParams->getParamId("analysis_sines"),
                                           sines);
    
    addAndMakeVisible(sinesLabel);
    sinesLabel.setText("Maximum Sinusoid Count", dontSendNotification);
    sinesLabel.attachToComponent(&sines, false);

}

AnalysisComponent::~AnalysisComponent()
{
}

void AnalysisComponent::paint (Graphics& g)
{
    g.setColour(LoomLookAndFeel::loomTextColour);
    g.drawText("Analysis Parameters", 0, 10, getWidth(), 15, Justification::centred);
}

void AnalysisComponent::resized()
{
    analysisButton.setBounds((getWidth()/2.0f) - (138/2.0f), 196, 138, 34);
    windowSize.setBounds(50, 72, 71, 20);
    
    // Hop size doesn't really work right now
    //hopSize.setBounds(35, 109, 71, 20);

    amplitudeTreshold.setBounds(156, 72, 200, 20);
    duration.setBounds(156, 139, 200, 20);
    sines.setBounds(394, 139, 200, 20);
    freqOffset.setBounds(394, 72, 200, 20);

    //freqSlope.setBounds(379, 109, 200, 20);
}


void AnalysisComponent::addComboBox(String paramId)
{
}


void AnalysisComponent::addComboBoxOptions(ComboBox* box, const String paramId)
{
    AudioProcessorValueTreeState* parameters = analysisParams->getParameters();
    AudioProcessorParameter* param = parameters->getParameter(analysisParams->getParamId(paramId));
    NormalisableRange<float> range = parameters->getParameterRange(analysisParams->getParamId(paramId));
    
    float i = range.start;
    while(i <= range.end)
    {
        box->addItem(param->getText(range.convertTo0to1(i), 20), roundToInt(i + 1.0f));
        i += range.interval;
    }
    
    box->setSelectedItemIndex(roundToInt((float)*parameters->getRawParameterValue(analysisParams->getParamId(paramId))));
}
