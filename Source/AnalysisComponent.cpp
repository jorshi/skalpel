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
    comboAttachment = new ComboBoxAttachment(*analysisParams->getParameters(),
                                             analysisParams->getParamId("analysis_window"), windowSize);
    
    addComboBoxOptions(&windowSize, "analysis_window");
}

AnalysisComponent::~AnalysisComponent()
{
}

void AnalysisComponent::paint (Graphics& g)
{
}

void AnalysisComponent::resized()
{
    analysisButton.setBounds(241, 166, 138, 34);
    windowSize.setBounds(106, 42, 90, 29);
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
