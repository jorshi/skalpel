/*
  ==============================================================================

    ModulationComponent.cpp
    Created: 2 Apr 2017 1:55:44am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ModulationComponent.h"

//==============================================================================
ModulationComponent::ModulationComponent(ActionListener* parent, EnvelopeParamaterManager* p) : params(p)
{
    // Setup action broadcaster
    broadcaster.removeAllActionListeners();
    broadcaster.addActionListener(parent);
    
    // Attack Time Rotary
    addAndMakeVisible(attack);
    attack.setTextValueSuffix(" ms");
    attack.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    attack.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
    attackAttachment = new SliderAttachment(*params->getParameters(),
                                            "adsr_1_attack",
                                            attack);
    
    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", dontSendNotification);
    attackLabel.attachToComponent(&attack, false);
    
    // Decay Time Rotary
    addAndMakeVisible(decay);
    decay.setTextValueSuffix(" ms");
    decay.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    decay.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
    decayAttachment = new SliderAttachment(*params->getParameters(),
                                            "adsr_1_decay",
                                            decay);
    
    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", dontSendNotification);
    decayLabel.attachToComponent(&decay, false);
    
    // Sustain Level Rotary
    addAndMakeVisible(sustain);
    sustain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sustain.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
    sustainAttachment = new SliderAttachment(*params->getParameters(),
                                             "adsr_1_sustain",
                                             sustain);
    
    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", dontSendNotification);
    sustainLabel.attachToComponent(&sustain, false);
    
    // Release Time Rotary
    addAndMakeVisible(release);
    release.setTextValueSuffix(" ms");
    release.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    release.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 15);
    releaseAttachment = new SliderAttachment(*params->getParameters(),
                                            "adsr_1_release",
                                             release);
    
    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", dontSendNotification);
    releaseLabel.attachToComponent(&release, false);
}


ModulationComponent::~ModulationComponent()
{
}


void ModulationComponent::paint (Graphics& g)
{
}


void ModulationComponent::resized()
{
    attack.setBounds(20, 25, 50, 55);
    decay.setBounds(80, 25, 50, 55);
    sustain.setBounds(140, 25, 50, 55);
    release.setBounds(200, 25, 50, 55);
}


void ModulationComponent::actionListenerCallback(const String &message)
{
    broadcaster.sendActionMessage(message);
}
