/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SinusoidalSynthSound.h"
#include "SinusoidalSynthVoice.h"
#include "SoundInterfaceManager.h"
#include "SoundInterface.h"
#include "AnalysisParameterManager.h"
#include "Modulation.h"
#include "ModulationFactory.h"
#include "ADSR.h"


//==============================================================================
/**
*/
class LoomAudioProcessor  : public AudioProcessor
{
public:
    enum
    {
        maxVoices = 4,
        maxSounds = 4
    };
    
    //==============================================================================
    LoomAudioProcessor();
    ~LoomAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void swapModel(ScopedPointer<SineModel> newModel);
    void swapSound(const SoundInterface& newSound);
    
    SoundInterface* getCurrentSound() { return soundManager_->getInterface(currentUISound_); };
    
    AudioProcessorValueTreeState& getParams();

private:
    //==============================================================================
    
    // Keep track of model currently showing on UI
    int currentUISound_;

    // Sound manager is responsible for all sound model and parameter management objects
    ScopedPointer<SoundInterfaceManager> soundManager_;
    
    Synthesiser synth_;
    
    ScopedPointer<UndoManager> undoManager_;
    ScopedPointer<AudioProcessorValueTreeState> parameters_;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomAudioProcessor)
};



#endif  // PLUGINPROCESSOR_H_INCLUDED
