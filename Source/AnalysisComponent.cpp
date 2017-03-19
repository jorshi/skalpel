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
    
    //addComboBox("analysis_frame_size");
}

AnalysisComponent::~AnalysisComponent()
{
}

void AnalysisComponent::paint (Graphics& g)
{
}

void AnalysisComponent::resized()
{
    //comboBoxMap.at("analysis_frame_size")->setBounds(106, 42, 90, 29);
    //analysisButton.setBounds(241, 166, 138, 34);
}


void AnalysisComponent::comboBoxChanged(juce::ComboBox *box)
{
}


void AnalysisComponent::addComboBox(String paramId)
{
}
