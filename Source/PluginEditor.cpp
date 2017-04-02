/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//TODO: convert this button listener with broadcasts from children to use the new action listener

//==============================================================================
LoomAudioProcessorEditor::LoomAudioProcessorEditor (LoomAudioProcessor& p) :
    AudioProcessorEditor (&p), processor (p), soundManager(p.getSoundInterfaceManager()),
    sounds(soundManager->getSounds())
{
    setLookAndFeel(&loomLookAndFeel);

    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    gradientMain = ColourGradient(Colour::fromRGB(63, 63, 63), 0.0f, 61.0f, Colour::fromRGB(43, 43, 43), 0.0f, 278.0f, false);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 690, 485);
    header = Rectangle<int>(0, 0, 690, 51);
    footer = Rectangle<int>(0, 411, 690, 89);
    middle = Rectangle<int>(10, 61, 670, 290);
    layers = Rectangle<int>(10, 358, 670, 46);
    
    SoundInterface* soundInterface;
    AnalysisComponent* analysisComp;
    LoadComponent* loadComp;
    SynthesisComponent* synthesisComp;
    
    for (int i = 0; i < sounds.size(); i++)
    {
        soundInterface = sounds.getUnchecked(i);
        
        // Create new UI components for each sound
        analysisComp = new AnalysisComponent(this, soundInterface->getAnalysisParams());
        loadComp = new LoadComponent(this, this, soundInterface);
        synthesisComp = new SynthesisComponent(this, soundInterface->getSynthParams());
        
        // Add each component to a managed array
        analysisComponents.add(analysisComp);
        loadComponents.add(loadComp);
        synthesisComponents.add(synthesisComp);
        
        // Add each component to the UI
        addAndMakeVisible(analysisComp);
        addAndMakeVisible(loadComp);
        addAndMakeVisible(synthesisComp);
    }
    
    modulationComponent = new ModulationComponent(this, soundManager->getEnvelopeParameterManager());
    addAndMakeVisible(modulationComponent);
    layeringComponent = new LayeringComponent(this);
    addAndMakeVisible(layeringComponent);
    
    loadCurrentSound();
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (690, 500);
}

LoomAudioProcessorEditor::~LoomAudioProcessorEditor()
{
}

//==============================================================================
void LoomAudioProcessorEditor::paint (Graphics& g)
{
    g.setColour(bgColour);
    g.fillRect(background);

    g.setColour(layer1Colour);
    g.fillRect(header);
    g.fillRect(footer);
    g.fillRect(layers);
    
    g.setGradientFill(gradientMain);
    g.fillRect(middle);
    
    g.setColour(Colours::black);
    g.drawRect(middle);
    g.drawRect(layers);
    g.drawHorizontalLine(header.getBottom(), header.getWidth() - header.getRight(), header.getRight());
    g.drawHorizontalLine(footer.getBottom() - footer.getHeight(), footer.getWidth() - footer.getRight(), footer.getRight());
    
    g.setColour(LoomLookAndFeel::loomTextColour);
    Font loomFont = Font(LoomLookAndFeel::daysOneTypeFace);
    loomFont.setHeight(51);
    g.setFont(loomFont);
    g.drawText("LOOM", 10, 0, 182, 51, Justification::left);
    
}

void LoomAudioProcessorEditor::resized()
{
    analysisComponents.getUnchecked(currentSound)->setBounds(middle);
    loadComponents.getUnchecked(currentSound)->setBounds(middle);
    synthesisComponents.getUnchecked(currentSound)->setBounds(middle);
    modulationComponent->setBounds(footer);
    layeringComponent->setBounds(layers);
}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    
    if (button->getComponentID() == "load_file")
    {
        currentSoundInterface->loadFile();
        loadSoundState();
    }
    
    if (button->getComponentID() == "run_analysis")
    {
        currentSoundInterface->runAnalysis();
        //processor.swapSound(soundInterface);
        switchSoundState(SoundInterface::synthesisState);
    }
    
    if (button->getComponentID() == "new_analysis")
    {
        switchSoundState(SoundInterface::loadFileState);
    }
    
    if (button->getComponentID() == "re-analysis")
    {
        switchSoundState(SoundInterface::analysisState);
    }
}


void LoomAudioProcessorEditor::switchSoundState(SoundInterface::State newState)
{
    if (currentSoundInterface->getState() != newState)
    {
        currentSoundInterface->setState(newState);
        loadSoundState();
    }
}


void LoomAudioProcessorEditor::hideMiddle()
{
    analysisComponents.getUnchecked(currentSound)->setVisible(false);
    loadComponents.getUnchecked(currentSound)->setVisible(false);
    synthesisComponents.getUnchecked(currentSound)->setVisible(false);
}


void LoomAudioProcessorEditor::loadSoundState()
{
    hideMiddle();
    switch(currentSoundInterface->getState())
    {
        case SoundInterface::loadFileState:
            loadComponents.getUnchecked(currentSound)->setVisible(true);
            break;
            
        case SoundInterface::analysisState:
            analysisComponents.getUnchecked(currentSound)->setVisible(true);
            break;
            
        case SoundInterface::runningAnalysisState:
            break;
            
        case SoundInterface::synthesisState:
            synthesisComponents.getUnchecked(currentSound)->setVisible(true);
            break;
    }
}


void LoomAudioProcessorEditor::actionListenerCallback(const String& message)
{
    if (message == "reload_state")
    {
        loadSoundState();
    }
}

void LoomAudioProcessorEditor::loadCurrentSound()
{
    hideAllSounds();
    currentSound = processor.getCurrentSoundNum();
    if (currentSound >= 0 && currentSound < sounds.size())
    {
        analysisComponents.getUnchecked(currentSound)->setVisible(true);
        loadComponents.getUnchecked(currentSound)->setVisible(true);
        synthesisComponents.getUnchecked(currentSound)->setVisible(true);
        currentSoundInterface = soundManager->getInterface(currentSound);
        loadSoundState();
    }
}

void LoomAudioProcessorEditor::hideAllSounds()
{
    for (int i = 0; i < sounds.size(); i++)
    {
        analysisComponents.getUnchecked(i)->setVisible(false);
        loadComponents.getUnchecked(i)->setVisible(false);
        synthesisComponents.getUnchecked(i)->setVisible(false);
    }
}


