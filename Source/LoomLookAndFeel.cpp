/*
  ==============================================================================

    LoomLookAndFeel.cpp
    Created: 18 Mar 2017 1:48:42am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "LoomLookAndFeel.h"


LoomLookAndFeel::LoomLookAndFeel() :
    buttonEdgeColour(Colour::fromRGB(124, 141, 165)),
    buttonTextColour(Colour::fromRGB(168, 168, 168))
{
    setColour(TextButton::textColourOnId, Colours::aliceblue);
    setColour(TextButton::textColourOffId, buttonTextColour);
}


void LoomLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                           bool isMouseOverButton, bool isButtonDown)
{
    Rectangle<float> buttonArea = button.getLocalBounds().toFloat();
    g.setColour(buttonEdgeColour);
    g.drawRoundedRectangle(buttonArea, 7.0f, 1.5f);
}

Font LoomLookAndFeel::getTextButtonFont (TextButton&, int buttonHeight)
{
    return Font (jmin (13.5f, buttonHeight * 0.6f));
}
