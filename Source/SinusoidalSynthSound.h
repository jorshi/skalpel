/*
  ==============================================================================

    SinusoidalSynthSound.h
    Created: 17 Feb 2017 4:03:26pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SINUSOIDALSYNTHSOUND_H_INCLUDED
#define SINUSOIDALSYNTHSOUND_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SineElement.h"
#include "marsyas/system/MarSystemManager.h"
#include "SynthesisUtils.h"
#include "SoundInterface.h"
#include "SoundInterfaceManager.h"


class SinusoidalSynthSound : public SynthesiserSound
{
public:
    
    // Constructor
    SinusoidalSynthSound(const BigInteger& notes, int midiNoteForNormalPitch,
                         int frameSize, SoundInterfaceManager* manager);
    
    ~SinusoidalSynthSound();
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    const int& getFrameSize() const { return frameSize_; };
    
    // Get a time domain frame at a requested time location
    bool getSignal(mrs_realvec&, mrs_real, int) const;
    
    // Get a reference to all playing sinusoid models
    ReferenceCountedArray<SineModel> getPlayingSineModels() const;
    
    // Sample the blackman harris window
    mrs_real getBH(int index) { return bh1001_(index); };
    
    // Sample the synthesis window function
    mrs_real getSynthWindow(int index) { return synthWindow_(index); };
    
    
    
private:
    friend class SinusoidalSynthVoice;
    
    // Reset all real and imaginary spectrum values to 0
    void resetSpectrum();
    
    BigInteger _midiNotes;
    int _midiRootNote;
    
    SineModel _model;
    mrs_realvec bh1001_;
    mrs_realvec synthWindow_;

    int frameSize_;
    mrs_real _nyquistBin;
    
    // Signals for holding spectral and time domain signals
    std::vector<FFT::Complex> _spectrum;
    std::vector<FFT::Complex> _timeSignal;
    
    // Pointer to FFT class
    ScopedPointer<FFT> _fftFunction;
    
    SoundInterfaceManager* manager_;
    
    JUCE_LEAK_DETECTOR(SinusoidalSynthSound)
};

#endif  // SINUSOIDALSYNTHSOUND_H_INCLUDED
