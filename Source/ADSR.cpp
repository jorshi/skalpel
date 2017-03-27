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
}


ADSR::ADSR(const ADSR& c)
{
    currentPhase_ = c.currentPhase_;
    position_ = c.position_;
    parameters_ = c.parameters_;
}


Modulation::Ptr ADSR::clone()
{
    return new ADSR(*this);
}


void ADSR::apply(float& value, int samples)
{
    position_ += samples;
}
