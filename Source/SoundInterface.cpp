/*
  ==============================================================================

    SoundInterface.cpp
    Created: 18 Mar 2017 12:28:33pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SoundInterface.h"


SoundInterface::SoundInterface() : analysis_(nullptr), state_(loadFileState)
{
    sineModel_ = new SineModel;
}


SoundInterface::~SoundInterface()
{
}


void SoundInterface::buildAnalysis(File inputFile)
{
    analysis_ = new AnalysisMrs(inputFile);
}


void SoundInterface::runAnalysis()
{
    if (analysis_ != nullptr)
    {
        sineModel_ = analysis_->runAnalysis();
    }
}
