/*
  ==============================================================================

    StochasticModel.h
    Created: 7 Apr 2017 6:56:13pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef STOCHASTICMODEL_H_INCLUDED
#define STOCHASTICMODEL_H_INCLUDED


#include "../JuceLibraryCode/JuceHeader.h"

class StochasticModel : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<StochasticModel> Ptr;
    typedef const ReferenceCountedObjectPtr<StochasticModel> ConstPtr;

    typedef std::vector<std::vector<float>> StochasticModelMatrix;
    typedef std::vector<float> StochasticModelFrame;
    
    // Constructor -- the model factor must be defined
    StochasticModel(int factor) : factor_(factor) {};
    
    // Defuault Deconstructor
    ~StochasticModel() {};
    
    // Hop size represented by this model
    void setHopSize(int hopSize) { hopSize_ = hopSize; };
    
    // Sample rate for this model
    void setSampleRate(float rate) { sampleRate_ = rate; };
    
    // Add a new frame from a complex spectrum
    void addFrame(const std::vector<FFT::Complex>& spectrum);
    
    // Get a frame 
    const StochasticModelFrame& getFrame(int index) const { return model_.at(index); };
    
    const int size() const { return model_.size(); };
    
private:
    
    StochasticModelMatrix model_;
    const int factor_;
    int hopSize_;
    float sampleRate_;
    
    JUCE_LEAK_DETECTOR(StochasticModel)
};




#endif  // STOCHASTICMODEL_H_INCLUDED
