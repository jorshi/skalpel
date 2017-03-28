/*
  ==============================================================================

    ADSR.cpp
    Created: 27 Mar 2017 12:18:27am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "ADSR.h"

ADSR::ADSR(AudioProcessorValueTreeState* p) : position_(0), parameters_(p), currentPhase_(off)
{
    attackTime_ = 100.0f;
    decayTime_ = 500.0f;
    sustainLevel_ = 0.0f;
}


ADSR::ADSR(const ADSR& c)
{
    currentPhase_ = c.currentPhase_;
    position_ = c.position_;
    parameters_ = c.parameters_;
    attackTime_ = c.attackTime_;
    decayTime_ = c.decayTime_;
    sustainLevel_ = c.sustainLevel_;
}


Modulation::Ptr ADSR::clone()
{
    return new ADSR(*this);
}


void ADSR::apply(float& value)
{
    float length;
    float factor;
    switch (currentPhase_)
    {
        case attack:
            length = getRate() * (attackTime_ / 1000);
            factor = (length > 0) ? (float(position_) / length) : 1.0f;
            break;
            
        case decay:
            length = getRate() * (decayTime_ / 1000);
            factor = (length > 0) ? 1.0f - ((float(position_) / length) * (1.0f - sustainLevel_)) : 1.0f;
            break;
            
        case sustain:
            factor = sustainLevel_;
            break;
        
        default:
            break;
    }
    
    value *= factor;
}

void ADSR::increment(int samples)
{
    position_ += samples;
    float timePos = float(position_) / getRate();
    
    switch (currentPhase_)
    {
        case attack:
            if (timePos >= (attackTime_ / 1000))
            {
                currentPhase_ = decay;
                position_ = 0;
            }
            break;
            
        case decay:
            if (timePos >= (decayTime_ / 1000))
            {
                currentPhase_ = sustain;
                position_ = 0;
            }
            break;
            
        default:
            break;
    }
}

void ADSR::turnOff()
{
    currentPhase_ = off;
    position_ = 0;
}
