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
#include "AnalysisComponent.h"
#include "LoadComponent.h"
#include "SynthesisComponent.h"


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
    
    // Called when the UI state changes
    void switchState(SoundInterface::State newState);
    
    void hideMiddle();
    
    void loadState();
    
    LoomAudioProcessor& processor;
    
    // Look and feel
    LoomLookAndFeel loomLookAndFeel;
    
    // Colours
    Colour bgColour;
    Colour layer1Colour;
    ColourGradient gradientMain;
    
    // Shapes
    Rectangle<int> header;
    Rectangle<int> background;
    Rectangle<int> footer;
    Rectangle<int> middle;
    
    // File Loader
    FileLoader fileLoader;
    
    // For interfacing with analysis and sounds
    SoundInterface& soundInterface;
    
    // This is where analysis and synthesis params go
    //MiddleComponent middleComponent;
    AnalysisComponent analysisComponent;
    LoadComponent loadComponent;
    SynthesisComponent synthesisComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
