/*
  ==============================================================================

    Analysis.h
    Created: 10 Feb 2017 12:12:10am
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ANALYSIS_H_INCLUDED
#define ANALYSIS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "essentia/algorithmfactory.h"

using namespace essentia;
using namespace essentia::streaming;


class Analysis
{
public:
    Analysis();
    ~Analysis();
    void newAnalysis();
    void setup();
    
private:
    
    int sampleRate;
    int frameSize;
    int hopSize;

    AudioSampleBuffer inputAudio;
    AudioFormatManager formatManager;
    
    //AlgorithmFactory* factory;
    //ScopedPointer<Algorithm> input;
};



#endif  // ANALYSIS_H_INCLUDED
