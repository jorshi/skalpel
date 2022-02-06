/*
  ==============================================================================

    SineElement.cpp
    Created: 11 Feb 2017 2:09:40pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SineElement.h"

// Default Constructor
SineElement::SineElement() : freq_(0.0), amp_(0.0), phase_(0.0), track_(-1), trackStart_(false)
{
}

SineElement::SineElement(double freq, double amp, double phase) :
freq_(freq), amp_(amp), phase_(phase), track_(-1), trackStart_(false)
{
}

SineElement::~SineElement()
{
}

void SineElement::setNewTrack(int track)
{
    track_ = track;
    trackStart_ = true;
}

//==============================================================================
// SineModel

SineModel::SineModel() : sampleRate_(0.0), frameSize_(0)
{
    sineModel_ = SineModel::SineMatrix();
}


SineModel::~SineModel()
{
}


void SineModel::addFrame(std::vector<SineElement> newFrame)
{
    sineModel_.emplace_back(newFrame);
}
