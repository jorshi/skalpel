/*
  ==============================================================================

    SineElement.h
    Created: 11 Feb 2017 2:09:40pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SINEELEMENT_H_INCLUDED
#define SINEELEMENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "marsyas/system/MarSystemManager.h"

using namespace Marsyas;

class SineElement
{
public:
    SineElement();
    SineElement(mrs_real freq, mrs_real amp, mrs_real phase);
    ~SineElement();

    
    // Getters
    mrs_real getFreq() const { return freq_; };
    mrs_real getAmp() const  { return amp_; };
    mrs_real getPhase() const { return phase_; };
    mrs_natural getTrack() const { return track_; };
    bool isFirstInTrack() const { return trackStart_; };
    
    
    // Setters
    void setFreq(mrs_real freq) { freq_ = freq; };
    void setAmp(mrs_real amp) { amp_ = amp; };
    void setPhase(mrs_real phase) { phase_ = phase; };
    void setTrack(mrs_natural track) { track_ = track; };
    void setNewTrack(mrs_natural track);

private:
    mrs_real freq_;
    mrs_real amp_;
    mrs_real phase_;
    mrs_natural track_;
    bool trackStart_;
};


class SineModel : public ReferenceCountedObject
{
public:
    
    // Useful types
    typedef std::vector<SineElement> SineFrame;
    typedef std::vector<std::vector<SineElement>> SineMatrix;
    typedef ReferenceCountedObjectPtr<SineModel> Ptr;
    typedef const ReferenceCountedObjectPtr<SineModel> ConstPtr;
    
    // Constructor
    SineModel();
    
    // Destructor
    ~SineModel();
    
    // Setters
    void setSampleRate(mrs_real sr) { sampleRate_ = sr; };
    void setFrameSize(mrs_natural fs) { frameSize_ = fs; };
    void setSineModel(SineMatrix newModel) { sineModel_ = newModel; };
    void setHopSize(mrs_natural h) { hopSize_ = h; };
    
    // Add a new frame to the end of the current model
    void addFrame(std::vector<SineElement>);
    
    // Getters
    mrs_real getSampleRate() const { return sampleRate_; };
    mrs_natural getFrameSize() const { return frameSize_; };
    mrs_natural getHopSize() const { return hopSize_; };
    const SineFrame& getFrame(int frame) const { return sineModel_.at(frame); };
    const SineMatrix& getModel() { return sineModel_; };
    const int size() const { return sineModel_.size(); };
    
    // Get iterators for the sine model
    std::vector<std::vector<SineElement>>::iterator begin() { return sineModel_.begin(); };
    std::vector<std::vector<SineElement>>::iterator end()   { return sineModel_.end(); };
    
    // Const iterators
    std::vector<std::vector<SineElement>>::const_iterator begin() const { return sineModel_.begin(); };
    std::vector<std::vector<SineElement>>::const_iterator end() const { return sineModel_.end(); };

private:
    SineMatrix sineModel_;
    mrs_real sampleRate_;
    mrs_natural frameSize_;
    mrs_natural hopSize_;
};

#endif  // SINEELEMENT_H_INCLUDED
