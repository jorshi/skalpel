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
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
    Font getTextButtonFont (TextButton&, int buttonHeight) override;
    
private:
    
    // Colours
    Colour buttonEdgeColour;
    Colour buttonTextColour;
    
};




#endif  // LOOMLOOKANDFEEL_H_INCLUDED
