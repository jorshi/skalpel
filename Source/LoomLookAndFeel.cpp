/*
  ==============================================================================

    LoomLookAndFeel.cpp
    Created: 18 Mar 2017 1:48:42am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "LoomLookAndFeel.h"

LoomLookAndFeel::LoomLookAndFeel()
{
    setColour(TextButton::textColourOnId, Colours::aliceblue);
    setColour(TextButton::textColourOffId, buttonTextColour);
    setColour(Label::textColourId, buttonTextColour);
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

Font LoomLookAndFeel::getLabelFont(Label& label)
{
    return Font (13.5f);
}

// Setup our colours
const Colour LoomLookAndFeel::buttonEdgeColour = Colour::fromRGB(124, 141, 165);
const Colour LoomLookAndFeel::buttonTextColour = Colour::fromRGB(168, 168, 168);
const Colour LoomLookAndFeel::loomTextColour = Colour::fromRGB(102, 105, 108);

// Custom Typeface
const Typeface::Ptr LoomLookAndFeel::daysOneTypeFace = Typeface::createSystemTypefaceFor(BinaryData::DaysOneRegular_ttf, BinaryData::DaysOneRegular_ttfSize);

const Typeface::Ptr LoomLookAndFeel::zapfDingbatsTypeface = Typeface::createSystemTypefaceFor(BinaryData::ZapfDingbats_ttf, BinaryData::ZapfDingbats_ttfSize);





