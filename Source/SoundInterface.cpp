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
    
    // Analysis Parameters
    analysisParams_.createParameter("analysis_frame_size", "Analysis Frame Size",
                                    NormalisableRange<float>(9,16,1), 11);
}


SoundInterface::~SoundInterface()
{
}


void SoundInterface::buildAnalysis(File inputFile)
{
    analysis_ = new AnalysisMrs(inputFile, analysisParams_);
}


void SoundInterface::runAnalysis()
{
    if (analysis_ != nullptr)
    {
        sineModel_ = analysis_->runAnalysis();
    }
}
