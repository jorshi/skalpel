/*
  ==============================================================================

    SineElement.cpp
    Created: 11 Feb 2017 2:09:40pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SineElement.h"

// Default Constructor
SineElement::SineElement() : _freq(0.0), _amp(0.0), _phase(0.0), _track(-1)
{

}

SineElement::SineElement(mrs_real freq, mrs_real amp, mrs_real phase) :
_freq(freq), _amp(amp), _phase(phase), _track(-1)
{
    
}

SineElement::~SineElement()
{
    
}

//==============================================================================
// SineModel

SineModel::SineModel() : _sampleRate(0.0), _frameSize(0)
{
    _sineModel = SineModel::SineMatrix();
}

SineModel::~SineModel()
{
}

void SineModel::addFrame(std::vector<SineElement> newFrame)
{
    _sineModel.emplace_back(newFrame);
}




