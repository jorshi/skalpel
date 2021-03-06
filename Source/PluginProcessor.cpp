/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LoomAudioProcessor::LoomAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Parameters are managed in JUCE's AudioProcessorValueTreeState which also
    // supports undo and redo
    undoManager_ = new UndoManager;
    parameters_ = new AudioProcessorValueTreeState(*this, undoManager_);
    
    // Create the sound manager object
    soundManager_ = new SoundInterfaceManager(maxSounds, parameters_);

    // Allocate voices for synthesizer
    for (int i = 0; i < maxVoices; ++i)
        synth_.addVoice (new SinusoidalSynthVoice(*soundManager_));
    
    // Add Sinusoidal Synth sound to synthesizer
    BigInteger midiNotes;
    midiNotes.setRange(0, 126, true);
    synth_.addSound(new SinusoidalSynthSound(midiNotes, 69, 512, soundManager_));
    
    // Initialize the current sound playing
    currentUISound_ = 0;
    
    // Setup the value tree for the parameters, no new parameters can be created after this
    parameters_->state = ValueTree(Identifier("LOOM"));
}

LoomAudioProcessor::~LoomAudioProcessor()
{
}

//==============================================================================
const String LoomAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LoomAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LoomAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double LoomAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LoomAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LoomAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LoomAudioProcessor::setCurrentProgram (int index)
{
}

const String LoomAudioProcessor::getProgramName (int index)
{
    return String();
}

void LoomAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LoomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    ignoreUnused(samplesPerBlock);
    synth_.setCurrentPlaybackSampleRate(sampleRate);

    // Sets sample rate for all modulation sources
    Modulation::setRate(sampleRate);
    SoundInterface::setRate(sampleRate);
}

void LoomAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LoomAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void LoomAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    synth_.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool LoomAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LoomAudioProcessor::createEditor()
{
    return new LoomAudioProcessorEditor (*this);
}

//==============================================================================
void LoomAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LoomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


// Swap the sinusoidal model out
void LoomAudioProcessor::swapModel(ScopedPointer<SineModel> newModel)
{
}


void LoomAudioProcessor::swapSound(const SoundInterface &newSound)
{
}

AudioProcessorValueTreeState& LoomAudioProcessor::getParams()
{
    return *parameters_;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LoomAudioProcessor();
}

