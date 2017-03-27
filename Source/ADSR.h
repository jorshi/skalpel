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
    enum phase
    {
        attack,
        decay,
        sustain,
        release,
        off
    };

    // Constructor
    ADSR();
    
    // Deconstructor()
    ~ADSR() {};
    
    // Return an instance of self
    Modulation::Ptr clone() override;
    
    // Return modulation value at current time and update
    void apply(float& value, int samples=1) override;
    
private:
    
    int position_;
};



#endif  // ADSR_H_INCLUDED
