/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class LoomAudioProcessorEditor  : public AudioProcessorEditor,
                                  public Button::Listener
{
public:
    LoomAudioProcessorEditor (LoomAudioProcessor&);
    ~LoomAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void openButtonClicked();

private:
    // Called when any button on UI is clicked
    void buttonClicked(Button* button) override;
    
    LoomAudioProcessor& processor;
    TextButton openButton;
    
    // Colours
    Colour bgColour;
    Colour layer1Colour;
    
    // Shapes
    Rectangle<int> header;
    Rectangle<int> background;
    Rectangle<int> footer;
    

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
