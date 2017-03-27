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


Modulation* ADSR::clone()
{
    return new ADSR;
}


float ADSR::tick(int samples)
{
    position_ += samples;
    return 0.0f;
}
