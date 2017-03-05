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
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using namespace Marsyas;

class SineElement
{
public:
    SineElement();
    SineElement(mrs_real freq, mrs_real amp, mrs_real phase);
    ~SineElement();
    
    // Getters
    mrs_real getFreq() const { return _freq; };
    mrs_real getAmp() const  { return _amp; };
    mrs_real getPhase() const { return _phase; };
    mrs_natural getTrack() const { return _track; };
    
    // Setters
    void setFreq(mrs_real freq) { _freq = freq; };
    void setAmp(mrs_real amp) { _amp = amp; };
    void setPhase(mrs_real phase) { _phase = phase; };
    void setTrack(mrs_natural track) { _track = track; };
    
private:
    mrs_real _freq;
    mrs_real _amp;
    mrs_real _phase;
    mrs_natural _track;
};


class SineModel
{
public:
    typedef std::vector<SineElement> SineFrame;
    typedef std::vector<std::vector<SineElement>> SineMatrix;
    
    SineModel();
    ~SineModel();
    
    // Setters
    void setSampleRate(mrs_real sr) { _sampleRate = sr; };
    void setFrameSize(mrs_natural fs) { _frameSize = fs; };
    void setSineModel(SineMatrix newModel) { _sineModel = newModel; };
    void setHopSize(mrs_natural h) { _hopSize = h; };
    
    // Add a new frame to the end of the current model
    void addFrame(std::vector<SineElement>);
    
    // Getters
    mrs_real getSampleRate() const { return _sampleRate; };
    mrs_natural getFrameSize() const { return _frameSize; };
    mrs_natural getHopSize() const { return _hopSize; };
    const SineFrame& getFrame(int frame) const { return _sineModel.at(frame); };
    const SineMatrix& getModel() { return _sineModel; };
    
    // Get iterators for the sine model
    std::vector<std::vector<SineElement>>::iterator begin() { return _sineModel.begin(); };
    std::vector<std::vector<SineElement>>::iterator end()   { return _sineModel.end(); };
    
    // Const iterators
    std::vector<std::vector<SineElement>>::const_iterator begin() const { return _sineModel.begin(); };
    std::vector<std::vector<SineElement>>::const_iterator end() const { return _sineModel.end(); };

private:
    SineMatrix _sineModel;
    mrs_real _sampleRate;
    mrs_natural _frameSize;
    mrs_natural _hopSize;
};


// TESTING
SineModel getTestModel();
SineModel getSawModel();



#endif  // SINEELEMENT_H_INCLUDED
