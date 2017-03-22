/*
  ==============================================================================

    LoomLookAndFeel.h
    Created: 18 Mar 2017 1:48:42am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef LOOMLOOKANDFEEL_H_INCLUDED
#define LOOMLOOKANDFEEL_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class LoomLookAndFeel : public LookAndFeel_V3
{
public:
    
    // Default Constructor
    LoomLookAndFeel();
    
    // Custom Buttons
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
    // Custom Button Text
    Font getTextButtonFont (TextButton&, int buttonHeight) override;
    
    // Colours    
    static const Colour buttonEdgeColour;
    static const Colour buttonTextColour;

private:
    
};




#endif  // LOOMLOOKANDFEEL_H_INCLUDED
