/*
  ==============================================================================

    Modulation.h
    Created: 27 Mar 2017 12:18:21am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MODULATION_H_INCLUDED
#define MODULATION_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


class Modulation : public ReferenceCountedObject
{
public:
    
    typedef ReferenceCountedObjectPtr<Modulation> Ptr;
    
    // Default Constructor
    Modulation() {};
    
    // Default Deconstructor
    virtual ~Modulation() {};
    
    // Must be implemented in subclasses
    virtual Ptr clone()=0;
    
    // Applies modulation to a value and then updates modulation function
    virtual void apply(float& value)=0;
    
    virtual void increment(int samples=1)=0;
    
    // Sample rate shared by all modulation sources
    static void setRate(int sampleRate) { sampleRate_ = sampleRate; };
    static int getRate() { return sampleRate_; };
    
private:
    
    static int sampleRate_;
};


// Null Modulation Class, will act as a sort of nullptr in the case a
// modulation source that doesn't exist was requested (for whatever reason)
// and we want to return something that will have no effect without breaking
class NullModulation : public Modulation
{
public:
    Modulation::Ptr clone() override { return new NullModulation; };
    void apply(float& value) override {};
    void increment(int samples=1) override {};
};




#endif  // MODULATION_H_INCLUDED
