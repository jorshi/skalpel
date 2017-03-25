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
    next_ = nullptr;
    prev_ = nullptr;
}

SineElement::SineElement(mrs_real freq, mrs_real amp, mrs_real phase) :
freq_(freq), amp_(amp), phase_(phase), track_(-1), trackStart_(false)
{
    next_ = nullptr;
    prev_ = nullptr;
}

SineElement::SineElement(const SineElement& s)
{
    freq_ = s.freq_;
    amp_ = s.amp_;
    phase_ = s.phase_;
    track_ = s.track_;
    trackStart_ = s.trackStart_;
    next_ = s.next_;
    prev_ = s.prev_;
}

SineElement::SineElement(const SineElement&& s)
{
    freq_ = s.freq_;
    amp_ = s.amp_;
    phase_ = s.phase_;
    track_ = s.track_;
    trackStart_ = s.trackStart_;
    next_ = s.next_;
    prev_ = s.prev_;
}

SineElement::~SineElement()
{
    if (prev_ != nullptr)
    {
        prev_->setNext(nullptr);
    }
    
    if (next_ != nullptr)
    {
        next_->setPrev(nullptr);
    }
}

SineElement& SineElement::operator=(const SineElement& s)
{
    freq_ = s.freq_;
    amp_ = s.amp_;
    phase_ = s.phase_;
    track_ = s.track_;
    trackStart_ = s.trackStart_;
    next_ = s.next_;
    prev_ = s.prev_;
    return *this;
}

SineElement& SineElement::operator=(SineElement&& s)
{
    freq_ = s.freq_;
    amp_ = s.amp_;
    phase_ = s.phase_;
    track_ = s.track_;
    trackStart_ = s.trackStart_;
    next_ = s.next_;
    prev_ = s.prev_;
    return *this;
}

void SineElement::setNewTrack(mrs_natural track)
{
    track_ = track;
    trackStart_ = true;
}

void SineElement::updateWithPrevious(SineElement *prevTrack)
{
    track_ = prevTrack->track_;
    prevTrack->next_ = this;
    prev_ = prevTrack;
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
