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
#include "FileLoader.h"
#include "AnalysisMrs.h"
#include "LoomLookAndFeel.h"
#include "SoundInterface.h"


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
    
    enum UIState
    {
        loadFileState,
        analysisState,
        synthesisState
    };
    
    // Called when any button on UI is clicked
    void buttonClicked(Button* button) override;
    
    // Called when the UI state changes
    void switchState(UIState newState);
    
    // Remove all buttons from UI
    void hideAllButtons();
    
    LoomAudioProcessor& processor;
    
    // Look and feel
    LoomLookAndFeel loomLookAndFeel;
    
    TextButton openButton;
    TextButton analysisButton;
    TextButton newButton;
    
    // Colours
    Colour bgColour;
    Colour layer1Colour;
    ColourGradient gradientMain;
    
    // Shapes
    Rectangle<int> header;
    Rectangle<int> background;
    Rectangle<int> footer;
    Rectangle<int> middle;
    
    // Current state of UI
    UIState state;
    
    // File Loader
    FileLoader fileLoader;
    
    // For interfacing with analysis and sounds
    SoundInterface& soundInterface;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
