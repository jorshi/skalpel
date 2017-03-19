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
AnalysisComponent::AnalysisComponent(ParameterManager& p) : analysisParams(p)
{
    ComboBox* newCombo;
    newCombo = new ComboBox("analysis_frame_size");
    comboBoxMap.emplace("analysis_frame_size", newCombo);
    comboBoxes.add(newCombo);
}

AnalysisComponent::~AnalysisComponent()
{
}

void AnalysisComponent::paint (Graphics& g)
{
}

void AnalysisComponent::resized()
{
    comboBoxMap.at("analysis_frame_size")->setBounds(106, 42, 90, 29);
}


void AnalysisComponent::comboBoxChanged(juce::ComboBox *box)
{
    
}
