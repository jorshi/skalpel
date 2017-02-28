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

SineModel getTestModel()
{
    mrs_natural hopSize = 128;
    mrs_real rate = 44100.0;
    
    SineModel testModel;
    testModel.setHopSize(hopSize);
    testModel.setFrameSize(hopSize);
    testModel.setSampleRate(rate);
    
    mrs_real freq = 440.0;
    mrs_real phase = 0.0;
    mrs_real phaseInc = (hopSize/rate)*freq*2*PI;
    
    
    
    for (int i = 0; i < 100; ++i)
    {
        SineElement newElement(440.0, -12.0, phase);
        phase += phaseInc;
        SineModel::SineFrame newFrame;
        newFrame.push_back(newElement);
        testModel.addFrame(newFrame);
    }
    
    return testModel;
}



