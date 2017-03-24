/*
  ==============================================================================

    SoundInterface.cpp
    Created: 18 Mar 2017 12:28:33pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SoundInterface.h"


SoundInterface::SoundInterface(AnalysisParameterManager* a) : analysis_(nullptr), state_(loadFileState)
{
    analysisParams_ = a;
    currentSineModel_ = new SineModel;
    sineModels_.add(currentSineModel_);
    isActive_ = false;
}


SoundInterface::~SoundInterface()
{
}


void SoundInterface::buildAnalysis(File inputFile)
{
    analysis_ = new AnalysisMrs(inputFile, *analysisParams_);
}


void SoundInterface::runAnalysis()
{
    if (analysis_ != nullptr)
    {
        currentSineModel_ = analysis_->runAnalysis();
        sineModels_.add(currentSineModel_);
        isActive_ = true;
    }
}


void SoundInterface::loadFile()
{
    ScopedPointer<File> file;
    if ((file = fileLoader_.getNewAudioFile()) != nullptr)
    {
        buildAnalysis(*file);
        state_ = analysisState;
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Audio Read Error", "Could not read input audio");
    }
}

void SoundInterface::loadFile(const juce::String &fileName)
{
    ScopedPointer<File> file = new File(fileName);
    if (fileLoader_.canReadFile(*file))
    {
        buildAnalysis(*file);
        state_ = analysisState;
    }
    else
    {
        AlertWindow::showMessageBox(AlertWindow::InfoIcon, "Audio Read Error", "Could not read input audio");
    }
}

bool SoundInterface::willAcceptFile(const juce::String &fileName)
{
    return fileLoader_.fileExtensionOkay(fileName);
}
