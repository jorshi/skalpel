/*
  ==============================================================================

    Modulation.h
    Created: 27 Mar 2017 12:18:21am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef MODULATION_H_INCLUDED
#define MODULATION_H_INCLUDED

class Modulation
{
public:
    
    // Default Constructor
    Modulation() {};
    
    // Default Deconstructor
    virtual ~Modulation() {};
    
    // Must be implemented in subclasses
    virtual Modulation* clone()=0;
    
    // Returns value at current time and updates
    virtual float tick(int samples=1)=0;
    
    // Sample rate shared by all modulation sources
    static void setRate(int sampleRate) { sampleRate_ = sampleRate; };
    static int getRate() { return sampleRate_; };
    
private:
    
    static int sampleRate_;
};




#endif  // MODULATION_H_INCLUDED
