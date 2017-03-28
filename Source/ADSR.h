/*
  ==============================================================================

    ADSR.h
    Created: 27 Mar 2017 12:18:27am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ADSR_H_INCLUDED
#define ADSR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "Modulation.h"

class ADSR : public Modulation
{
public:
    enum Phase
    {
        attack,
        decay,
        sustain,
        release,
        off
    };

    // Constructor
    ADSR(AudioProcessorValueTreeState* p);
    
    // Copy Constructor
    ADSR(const ADSR& c);
    
    // Deconstructor()
    ~ADSR() {};
    
    // Return an instance of self
    Modulation::Ptr clone() override;
    
    // Return modulation value at current time and update
    void apply(float& value) override;
    
    void increment(int samples=1) override;
    
    // Register parameter names for accessing in the parameters value tree
    void registerAttackParam(String id) { attackTimeParam_ = id; };
    
    void setPhase(Phase p) { currentPhase_ = p; };
    
    void turnOff();
    
private:
    
    int position_;
    AudioProcessorValueTreeState* parameters_;
    Phase currentPhase_;
    
    // Parameter mappings
    String attackTimeParam_;
    
    float attackTime_;
    float decayTime_;
    float sustainLevel_;
    float releaseTime_;
    
};



#endif  // ADSR_H_INCLUDED
