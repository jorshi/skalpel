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
AnalysisComponent::AnalysisComponent(ButtonListener* parent, AnalysisParameterManager* p) : analysisParams(p)
{
    analysisButton.setButtonText("Run Analysis");
    analysisButton.setComponentID("run_analysis");
    analysisButton.addListener(parent);
    addAndMakeVisible(&analysisButton);
    
    addAndMakeVisible(windowSize);
    addComboBoxOptions(&windowSize, "analysis_window");
    windowAttachment = new ComboBoxAttachment(*analysisParams->getParameters(),
                                              analysisParams->getParamId("analysis_window"),
                                              windowSize);
    
    addAndMakeVisible(hopSize);
    addComboBoxOptions(&hopSize, "analysis_hop");
    hopAttachment = new  ComboBoxAttachment(*analysisParams->getParameters(),
                                            analysisParams->getParamId("analysis_hop"),
                                            hopSize);
    
    addAndMakeVisible(amplitudeTreshold);
    amplitudeAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                               analysisParams->getParamId("analysis_amp_thresh"),
                                               amplitudeTreshold);
    
    addAndMakeVisible(duration);
    durationAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                              analysisParams->getParamId("analysis_duration"),
                                              duration);
    
    addAndMakeVisible(freqOffset);
    freqOffsetAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                               analysisParams->getParamId("analysis_freq_offset"),
                                               freqOffset);
    
    addAndMakeVisible(freqSlope);
    freqSlopeAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                               analysisParams->getParamId("analysis_freq_slope"),
                                               freqSlope);
    
    addAndMakeVisible(sines);
    sinesAttachment = new SliderAttachment(*analysisParams->getParameters(),
                                           analysisParams->getParamId("analysis_sines"),
                                           sines);

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
    analysisButton.setBounds(241, 166, 138, 34);
    windowSize.setBounds(35, 52, 71, 20);
    
    // Hop size doesn't really work right now
    //hopSize.setBounds(35, 109, 71, 20);
    
    sines.setBounds(35, 109, 100, 20);
    amplitudeTreshold.setBounds(141, 52, 200, 20);
    duration.setBounds(141, 109, 200, 20);
    freqOffset.setBounds(379, 52, 200, 20);
    freqSlope.setBounds(379, 109, 200, 20);
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

    box->setSelectedItemIndex(roundToInt(*parameters->getRawParameterValue(analysisParams->getParamId(paramId))));
}
