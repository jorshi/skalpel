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


class SineElement
{
public:
    SineElement();
    SineElement(double freq, double amp, double phase);
    ~SineElement();

    
    // Getters
    double getFreq() const { return freq_; };
    double getAmp() const  { return amp_; };
    double getPhase() const { return phase_; };
    int getTrack() const { return track_; };
    bool isFirstInTrack() const { return trackStart_; };
    
    
    // Setters
    void setFreq(double freq) { freq_ = freq; };
    void setAmp(double amp) { amp_ = amp; };
    void setPhase(double phase) { phase_ = phase; };
    void setTrack(int track) { track_ = track; };
    void setNewTrack(int track);

private:
    double freq_;
    double amp_;
    double phase_;
    int track_;
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
    void setSampleRate(double sr) { sampleRate_ = sr; };
    void setFrameSize(int fs) { frameSize_ = fs; };
    void setSineModel(SineMatrix newModel) { sineModel_ = newModel; };
    void setHopSize(int h) { hopSize_ = h; };
    
    // Add a new frame to the end of the current model
    void addFrame(std::vector<SineElement>);
    
    // Getters
    double getSampleRate() const { return sampleRate_; };
    int getFrameSize() const { return frameSize_; };
    int getHopSize() const { return hopSize_; };
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
    double sampleRate_;
    int frameSize_;
    int hopSize_;
};

#endif  // SINEELEMENT_H_INCLUDED
