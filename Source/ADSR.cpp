/*
  ==============================================================================

    ADSR.cpp
    Created: 27 Mar 2017 12:18:27am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "ADSR.h"

ADSR::ADSR() : position_(0)
{
}


Modulation::Ptr ADSR::clone()
{
    return new ADSR;
}


void ADSR::apply(float& value, int samples)
{
    position_ += samples;
}
