/*
  ==============================================================================

    SoundInterface.cpp
    Created: 18 Mar 2017 12:28:33pm
    Author:  Jordie Shier

  ==============================================================================
*/

#include "SoundInterface.h"


SoundInterface::SoundInterface(AnalysisParameterManager* a, SynthesisParameterManager* s) :
    analysis_(nullptr), stochasticAnalysis_(nullptr), state_(loadFileState)
{
    analysisParams_ = a;
    synthParams_ = s;

    // Create an empty sine model and add it to the owned array of models
    currentSineModel_ = new SineModel;
    sineModels_.add(currentSineModel_);

    // Default state is not playing
    isActive_ = false;
}


SoundInterface::~SoundInterface()
{
}


void SoundInterface::buildAnalysis(File inputFile)
{
    analysis_ = new AnalysisMrs(inputFile, *analysisParams_);
    stochasticAnalysis_ = new StochasticAnalysis(inputFile, *analysisParams_);
}


void SoundInterface::runAnalysis()
{
    if (analysis_ != nullptr)
    {
        currentSineModel_ = analysis_->runAnalysis();
        sineModels_.add(currentSineModel_);

        currentStochasticModel_ = stochasticAnalysis_->runAnalysis(currentSineModel_);
        stochasticModels_.add(currentStochasticModel_);

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


void SoundInterface::checkModels()
{
    for (int i = sineModels_.size(); --i >= 0;)
    {
        SineModel::Ptr model(sineModels_.getUnchecked(i));

        if (model->getReferenceCount() == 2)
        {
            sineModels_.remove(i);
        }
    }

    for (int i = stochasticModels_.size(); --i >= 0;)
    {
        StochasticModel::Ptr model(stochasticModels_.getUnchecked(i));

        if (model->getReferenceCount() == 2)
        {
            stochasticModels_.remove(i);
        }
    }
}



