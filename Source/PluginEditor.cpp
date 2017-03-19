/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LoomAudioProcessorEditor::LoomAudioProcessorEditor (LoomAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), soundInterface(p.getCurrentSound())
{
    
    setLookAndFeel(&loomLookAndFeel);
    

    AudioProcessorValueTreeState& tree = processor.getParams();
    
    Slider* newSlider;
    sliders.add(newSlider = new Slider("analysis_win"));
    
    sliderAttachments.add(new AudioProcessorValueTreeState::SliderAttachment(tree, "analysis_win", *newSlider));
    addAndMakeVisible(newSlider);
    
    
    ComboBox* newCombo;
    combos.add(newCombo = new ComboBox("analysis_win_1"));
    AudioProcessorParameter* param = tree.getParameter("analysis_win_1");
    
    newCombo->addItem(param->getText(0.0f, 20), 1);
    newCombo->addItem(param->getText(1.0f, 20), 2);
    
    comboAttachments.add(new AudioProcessorValueTreeState::ComboBoxAttachment(tree, "analysis_win_1", *newCombo));
    
    addAndMakeVisible(newCombo);
    
//    ToggleButton* newToggle;
//    buttons.add(newToggle = new ToggleButton("analysis_win_1"));
//    
//    buttonAttachments.add(new AudioProcessorValueTreeState::ButtonAttachment(tree, "analysis_win_1", *newToggle));
//    addAndMakeVisible(newToggle);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (640, 360);
    
    // Setup buttons
    openButton.setButtonText("Load File");
    openButton.addListener(this);
    addAndMakeVisible(&openButton);
    
    analysisButton.setButtonText("Run Analysis");
    analysisButton.addListener(this);
    addAndMakeVisible(&analysisButton);
    
    newButton.setButtonText("New Analysis");
    newButton.addListener(this);
    addAndMakeVisible(&newButton);
    
    // Load current state (defined in sound interface)
    loadState();
    
    // Setup Colours
    bgColour = Colour::fromRGB(71, 75, 81);
    layer1Colour = Colour::fromRGB(47, 47, 47);
    gradientMain = ColourGradient(Colour::fromRGB(63, 63, 63), 0.0f, 61.0f, Colour::fromRGB(43, 43, 43), 0.0f, 278.0f, false);
    
    // Create shapes
    background = Rectangle<int>(0, 0, 640, 360);
    header = Rectangle<int>(0, 0, 640, 51);
    footer = Rectangle<int>(0, 288, 640, 74);
    middle = Rectangle<int>(10, 61, 620, 217);
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
    
    g.setGradientFill(gradientMain);
    g.fillRect(middle);
    
    g.setColour(Colours::black);
    g.drawRect(middle);
    g.drawHorizontalLine(header.getBottom(), header.getWidth() - header.getRight(), header.getRight());
    g.drawHorizontalLine(footer.getBottom() - footer.getHeight(), footer.getWidth() - footer.getRight(), footer.getRight());
    
    // State dependent painting
    switch (soundInterface.getState()) {
        case SoundInterface::runningAnalysisState:
            g.drawText("Running Analysis", 0, 0, 50, 10, Justification::centred);
            break;
            
        default:
            break;
    }
}

void LoomAudioProcessorEditor::resized()
{
    openButton.setBounds(251, 163, 138, 34);
    analysisButton.setBounds(251, 227, 138, 34);
    newButton.setBounds(251, 227, 138, 34);
    
    sliders[0]->setBounds(0, 0, 100, 50);
    combos[0]->setBounds(50, 50, 200, 50);
    //buttons[0]->setBounds(50, 50, 50, 50);
}

void LoomAudioProcessorEditor::buttonClicked(Button* button)
{
    
    if (button == &openButton)
    {
        ScopedPointer<File> file;
        if ((file = fileLoader.getNewAudioFile()) != nullptr)
        {
            soundInterface.buildAnalysis(*file);
            switchState(SoundInterface::analysisState);
        }
        else
        {
            AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Audio Read Error", "Could not read input audio");
        }
    }
    
    if (button == &analysisButton)
    {
        //switchState(SoundInterface::runningAnalysisState);
        soundInterface.runAnalysis();
        processor.swapSound(soundInterface);
        switchState(SoundInterface::synthesisState);
    }
    
    if (button == &newButton)
    {
        switchState(SoundInterface::loadFileState);
    }
}


void LoomAudioProcessorEditor::switchState(SoundInterface::State newState)
{
    if (soundInterface.getState() != newState)
    {
        soundInterface.setState(newState);
        loadState();
    }
}


void LoomAudioProcessorEditor::hideAllButtons()
{
    openButton.setVisible(false);
    newButton.setVisible(false);
    analysisButton.setVisible(false);
}


void LoomAudioProcessorEditor::loadState()
{
    hideAllButtons();
    switch(soundInterface.getState())
    {
        case SoundInterface::loadFileState:
            openButton.setVisible(true);
            break;
            
        case SoundInterface::analysisState:
            analysisButton.setVisible(true);
            break;
            
        case SoundInterface::runningAnalysisState:
            repaint();
            break;
            
        case SoundInterface::synthesisState:
            newButton.setVisible(true);
            break;
    }
}


