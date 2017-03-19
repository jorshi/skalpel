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
AnalysisComponent::AnalysisComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

AnalysisComponent::~AnalysisComponent()
{
}

void AnalysisComponent::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (Colours::white);   // clear the background

    g.setColour (Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::lightblue);
    g.setFont (14.0f);
    g.drawText ("AnalysisComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void AnalysisComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
