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
    const int imageWidth = spectrogramImage.getWidth();
    const int imageHeight = spectrogramImage.getHeight();
    for (int i = 1; i < imageWidth; i++)
    {
        for (int j = 1; j < imageHeight; j++)
        {
            spectrogramImage.setPixelAt(i, j, LoomLookAndFeel::gradient.getColourAtPosition(float(j) / imageHeight));
        }
    }
}

VisualizerComponent::~VisualizerComponent()
{
}

void VisualizerComponent::paint (Graphics& g)
{

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
            spectrogramImage.setPixelAt (rightHandEdge, y, LoomLookAndFeel::gradient.getColourAtPosition(float(y) / imageHeight));
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

    float hue, saturation, brightness;
    LoomLookAndFeel::spectraColour.getHSB(hue, saturation, brightness);
    Colour pixelColour;
    
    for (int y = 1; y < imageHeight; y++)
    {
        const float skewedProportionY = 1.0f - std::exp (std::log (y / (float) imageHeight) * 0.175f);
        const int dataIndex = jlimit (0, size / 2, (int) (skewedProportionY * size / 2));
        const float level = jmap (frame[dataIndex], 0.0f, maxLevel, 0.0f, 1.0f);
        if (level > 0.001)
        {
            pixelColour = Colour::fromHSV(hue, saturation, 1.0f, level);

        }
        else
        {
            pixelColour = LoomLookAndFeel::gradient.getColourAtPosition(float(y) / imageHeight);
        }
        spectrogramImage.setPixelAt (rightHandEdge, y, pixelColour);
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
