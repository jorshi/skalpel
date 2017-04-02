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
#include "ModulationComponent.h"
#include "SoundInterfaceManager.h"


//==============================================================================
/**
*/
class LoomAudioProcessorEditor  : public AudioProcessorEditor,
                                  public Button::Listener,
                                  public ActionListener
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
    
    // Called when a child notifies of a change
    void actionListenerCallback(const String& message) override;
    
    // Called when the UI state changes
    void switchSoundState(SoundInterface::State newState);
    
    // Hide all the main sound components for the current sound
    void hideMiddle();
    
    // Load a new sound state
    void loadSoundState();
    
    // Get the current sound to be displayed on the main UI
    void loadCurrentSound();
    
    void hideAllSounds();
    
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
    Rectangle<int> layers;
    Rectangle<int> footer;
    Rectangle<int> middle;
    
    // File Loader
    FileLoader fileLoader;
    
    // For interfacing with analysis and sounds
    SoundInterfaceManager* soundManager;
    SoundInterface* currentSoundInterface;
    
    // This is where analysis and synthesis params go
    OwnedArray<AnalysisComponent> analysisComponents;
    OwnedArray<LoadComponent> loadComponents;
    OwnedArray<SynthesisComponent> synthesisComponents;
    ScopedPointer<ModulationComponent> modulationComponent;
    
    int currentSound;
    
    const OwnedArray<SoundInterface>& sounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
