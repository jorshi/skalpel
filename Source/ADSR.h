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
    void registerAttackParam(const String& id) { attackTimeParam_ = id; };
    
    void registerDecayParam(const String& id) { decayTimeParam_ = id; };
    
    void registerSustainParam(const String& id) { sustainLevelParam_ = id; };
    
    void registerReleaseParam(const String& id) { releaseTimeParam_ = id; };
    
    // Set the phase of the envelope (i.e. attack/decay/sustain/release)
    void setPhase(Phase p) { currentPhase_ = p; };
    
    void turnOff();
    
    // Set envelope into release stage
    void triggerRelease();
    
    // Trigger onset of envelope
    void triggerAttack();
    
    
private:
    
    int position_;
    AudioProcessorValueTreeState* parameters_;
    Phase currentPhase_;
    
    // Parameter mappings
    String attackTimeParam_;
    String decayTimeParam_;
    String sustainLevelParam_;
    String releaseTimeParam_;
    
    float attackTime_;
    float decayTime_;
    float sustainLevel_;
    float releaseLevel_;
    float releaseTime_;
    float currentLevel_;
    
};



#endif  // ADSR_H_INCLUDED
