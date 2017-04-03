/*
  ==============================================================================

    VisualizerComponent.cpp
    Created: 2 Apr 2017 10:17:06pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "VisualizerComponent.h"

//==============================================================================
VisualizerComponent::VisualizerComponent(SoundInterface* s) :
    sound(s), spectrogramImage(Image::RGB, 512, 512, true)
{
}

VisualizerComponent::~VisualizerComponent()
{
}

void VisualizerComponent::paint (Graphics& g)
{
    g.setColour (Colours::lightblue);
    g.setFont (14.0f);
    g.drawText ("VisualizerComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
    
    g.drawImage(spectrogramImage, getLocalBounds().toFloat());
}

void VisualizerComponent::resized()
{
}

void VisualizerComponent::timerCallback()
{
    if (sound->getVisualize())
    {
        drawNextLine();
        sound->setVisualize(false);
        sound->clearVisualizerFrame();
        repaint();
    }
    else
    {
        const int rightHandEdge = spectrogramImage.getWidth() - 1;
        const int imageHeight = spectrogramImage.getHeight();
        spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);
        for (int y = 1; y < imageHeight; y++)
        {
            spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSV (0.0f, 1.0f, 0.0f, 1.0f));
        }
        repaint();
    }
}

void VisualizerComponent::drawNextLine()
{
    const int rightHandEdge = spectrogramImage.getWidth() - 1;
    const int imageHeight = spectrogramImage.getHeight();
    
    spectrogramImage.moveImageSection(0, 0, 1, 0, rightHandEdge, imageHeight);
 
    const std::vector<float>& frame = sound->getVisualizerFrame();
    const int size = frame.size();
    
    float maxLevel;
    if (!((maxLevel = *std::max_element(frame.begin(), frame.end())) > 0))
    {
        return;
    }

    for (int y = 1; y < imageHeight; y++)
    {
        const float skewedProportionY = 1.0f - std::exp (std::log (y / (float) imageHeight) * 0.2f);
        const int dataIndex = jlimit (0, size / 2, (int) (skewedProportionY * size / 2));
        const float level = jmap (frame[dataIndex], 0.0f, maxLevel, 0.0f, 1.0f);
        spectrogramImage.setPixelAt (rightHandEdge, y, Colour::fromHSV (level, 1.0f, level, 1.0f));
    }
    
}

void VisualizerComponent::run()
{
    startTimerHz(60);
}

void VisualizerComponent::stop()
{
    stopTimer();
}
