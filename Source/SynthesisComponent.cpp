/*
  ==============================================================================

    SynthesisComponent.cpp
    Created: 20 Mar 2017 10:38:23pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SynthesisComponent.h"

//==============================================================================
SynthesisComponent::SynthesisComponent(ButtonListener* parent, SynthesisParameterManager* p) : synthesisParams(p)
{
    setLookAndFeel(&loomLookAndFeel);
    
    newButton.setButtonText("New");
    newButton.setComponentID("new_analysis");
    newButton.addListener(parent);
    addAndMakeVisible(&newButton);
    
    analysisButton.setButtonText("Analysis");
    analysisButton.setComponentID("re-analysis");
    analysisButton.addListener(parent);
    addAndMakeVisible(&analysisButton);
    
    visualizer = Rectangle<int>(120, 7, 445, 169);
    
    // Octave Tuning Rotary
    addAndMakeVisible(octave);
    octave.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    octave.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    octaveAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                            synthesisParams->getParamId("octave_tune"),
                                            octave);
    
    addAndMakeVisible(octaveLabel);
    octaveLabel.setText("Octave", dontSendNotification);
    octaveLabel.attachToComponent(&octave, false);
    
    // Semitone Tuner Rotary
    addAndMakeVisible(semitone);
    semitone.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    semitone.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    semitoneAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                            synthesisParams->getParamId("semitone_tune"),
                                            semitone);
    
    addAndMakeVisible(semitoneLabel);
    semitoneLabel.setText("Semitone", dontSendNotification);
    semitoneLabel.attachToComponent(&semitone, false);
    
    // Cent Tuner Rotary
    addAndMakeVisible(cent);
    cent.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    cent.setTextBoxStyle(Slider::TextBoxBelow, false, 40, 15);
    centAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                          synthesisParams->getParamId("cent_tune"),
                                          cent);
    
    addAndMakeVisible(centLabel);
    centLabel.setText("Cent", dontSendNotification);
    centLabel.attachToComponent(&cent, false);
    
    // Freqeuncy Multiply Rotary
    addAndMakeVisible(multiply);
    multiply.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    multiply.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    multiplyAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                              synthesisParams->getParamId("frequency_scale"),
                                              multiply);
    
    addAndMakeVisible(multiplyLabel);
    multiplyLabel.setText("Multiply", dontSendNotification);
    multiplyLabel.attachToComponent(&multiply, false);
    
    // Freqeuncy Shift Rotary
    addAndMakeVisible(shift);
    shift.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    shift.setTextValueSuffix(" Hz");
    shift.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    shiftAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                           synthesisParams->getParamId("freq_shift"),
                                           shift);
    
    addAndMakeVisible(shiftLabel);
    shiftLabel.setText("Shift", dontSendNotification);
    shiftLabel.attachToComponent(&shift, false);
    
    // Warp Center Rotary
    addAndMakeVisible(warpCenter);
    warpCenter.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    warpCenter.setTextValueSuffix(" Hz");
    warpCenter.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    warpCenterAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                                synthesisParams->getParamId("stretch_center"),
                                                warpCenter);
    
    addAndMakeVisible(warpCenterLabel);
    warpCenterLabel.setText("Center", dontSendNotification);
    warpCenterLabel.attachToComponent(&warpCenter, false);
    
    // Warp Amount Rotary
    addAndMakeVisible(warpAmount);
    warpAmount.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    warpAmount.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    warpAmountAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                                synthesisParams->getParamId("stretch_factor"),
                                                warpAmount);
    
    addAndMakeVisible(warpAmountLabel);
    warpAmountLabel.setText("Amount", dontSendNotification);
    warpAmountLabel.attachToComponent(&warpAmount, false);
    
    // Time Offset Rotary
    addAndMakeVisible(timeOffset);
    timeOffset.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    timeOffset.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    timeOffsetAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                                synthesisParams->getParamId("start_offset"),
                                                timeOffset);
    
    addAndMakeVisible(timeOffsetLabel);
    timeOffsetLabel.setText("Start", dontSendNotification);
    timeOffsetLabel.attachToComponent(&timeOffset, false);
    
    // Playback Rate Rotary
    addAndMakeVisible(playbackRate);
    playbackRate.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    playbackRate.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    playbackRateAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                                  synthesisParams->getParamId("playback_rate"),
                                                  playbackRate);
    
    addAndMakeVisible(playbackRateLabel);
    playbackRateLabel.setText("Rate", dontSendNotification);
    playbackRateLabel.attachToComponent(&playbackRate, false);
    
    // Sine Count Rotary
    addAndMakeVisible(sineCount);
    sineCount.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sineCount.setTextValueSuffix("");
    sineCount.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    sineCountAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                                synthesisParams->getParamId("sine_ratio"),
                                                sineCount);
    
    addAndMakeVisible(sineCountLabel);
    sineCountLabel.setText("Sines", dontSendNotification);
    sineCountLabel.attachToComponent(&sineCount, false);
    
    // Sine Gain Rotary
    addAndMakeVisible(sineGain);
    sineGain.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    sineGain.setTextValueSuffix(" dB");
    sineGain.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 17);
    sineGainAttachment = new SliderAttachment(*synthesisParams->getParameters(),
                                               synthesisParams->getParamId("sine_gain"),
                                               sineGain);
    
    addAndMakeVisible(sineGainLabel);
    sineGainLabel.setText("Gain", dontSendNotification);
    sineGainLabel.attachToComponent(&sineGain, false);
}


SynthesisComponent::~SynthesisComponent()
{
}


void SynthesisComponent::paint (Graphics& g)
{
    g.setColour(LoomLookAndFeel::boundaryColour);
    g.drawRect(visualizer, 2.0f);
    
    g.setColour(LoomLookAndFeel::loomTextColour);
    g.drawLine(182.5, 190, 182.5, 270, 2.0f);
    g.drawLine(332.5, 190, 332.5, 270, 2.0f);
    g.drawLine(482.5, 190, 482.5, 270, 2.0f);
    
    g.drawText("Tuning", 0, 5, 120, 15, Justification::centred);
    g.drawText("Frequency", 40, 180, 130, 15, Justification::centred);
    g.drawText("Spectral Warp", 190, 180, 130, 15, Justification::centred);
    g.drawText("Time Modulation", 345, 180, 130, 15, Justification::centred);
    g.drawText("Sine Model", 480, 180, 130, 15, Justification::centred);
}


void SynthesisComponent::resized()
{
    newButton.setBounds(577, 7, 85, 27);
    analysisButton.setBounds(577, 40, 85, 27);
    
    octave.setBounds(10, 45, 50, 55);
    semitone.setBounds(65, 45, 50, 55);
    cent.setBounds(40, 125, 40, 45);
    
    multiply.setBounds(50, 220, 50, 55);
    shift.setBounds(115, 220, 50, 55);
    
    warpCenter.setBounds(200, 220, 50, 55);
    warpAmount.setBounds(265, 220, 50, 55);
    
    timeOffset.setBounds(350, 220, 50, 55);
    playbackRate.setBounds(415, 220, 50, 55);
    
    sineCount.setBounds(500, 220, 50, 55);
    sineGain.setBounds(565, 220, 50, 55);
}
