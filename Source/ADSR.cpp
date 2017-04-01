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
    attackTime_ = 10.0f;
    decayTime_ = 250.0f;
    sustainLevel_ = 0.25f;
    releaseTime_ = 500.0f;
    currentLevel_ = 0.0f;
    setActive(false);
}


ADSR::ADSR(const ADSR& c)
{
    currentPhase_ = c.currentPhase_;
    position_ = c.position_;
    parameters_ = c.parameters_;
    attackTime_ = c.attackTime_;
    decayTime_ = c.decayTime_;
    sustainLevel_ = c.sustainLevel_;
    releaseTime_ = c.releaseTime_;
    
    attackTimeParam_ = c.attackTimeParam_;
    decayTimeParam_ = c.decayTimeParam_;
    sustainLevelParam_ = c.sustainLevelParam_;
    releaseTimeParam_ = c.releaseTimeParam_;
}


Modulation::Ptr ADSR::clone()
{
    return new ADSR(*this);
}


void ADSR::apply(float& value)
{
    float length;
    float* param;
    switch (currentPhase_)
    {
        case attack:
            if ((param = parameters_->getRawParameterValue(attackTimeParam_)))
            {
                attackTime_ = *param;
            }
            length = getRate() * (attackTime_/ 1000);
            currentLevel_ = (length > 0) ? (float(position_) / length) : 1.0f;
            break;
            
        case decay:
            if ((param = parameters_->getRawParameterValue(decayTimeParam_)))
            {
                decayTime_ = *param;
            }
            if ((param = parameters_->getRawParameterValue(sustainLevelParam_)))
            {
                sustainLevel_ = *param;
            }
            length = getRate() * (decayTime_ / 1000);
            currentLevel_ = (length > 0) ? 1.0f - ((float(position_) / length) * (1.0f - sustainLevel_)) : 1.0f;
            break;
            
        case sustain:
            if ((param = parameters_->getRawParameterValue(sustainLevelParam_)))
            {
                sustainLevel_ = *param;
            }
            currentLevel_ = sustainLevel_;
            break;
            
        case release:
            if ((param = parameters_->getRawParameterValue(releaseTimeParam_)))
            {
                releaseTime_  = *param;
            }
            length = getRate() * (releaseTime_ / 1000);
            currentLevel_ = (length > 0) ? (1.0f - (float(position_) / length)) * sustainLevel_ : 0.0f;
            break;
            
        case off:
            currentLevel_ = 0.0f;
            break;

        default:
            currentLevel_ = 1.0;
            break;
    }
    
    value *= currentLevel_;
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
            
        case release:
            if (timePos >= (releaseTime_ / 1000))
            {
                setActive(false);
                currentPhase_ = off;
                position_ = 0;
            }
            
        default:
            break;
    }
}

void ADSR::turnOff()
{
    currentPhase_ = off;
    position_ = 0;
}


void ADSR::triggerAttack()
{
    setActive(true);
    currentPhase_ = attack;
    currentLevel_ = 0.0f;
    position_ = 0;
}


void ADSR::triggerRelease()
{
    currentPhase_ = release;
    releaseLevel_ = currentLevel_;
    position_ = 0;
}


