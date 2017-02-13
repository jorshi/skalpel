/*
  ==============================================================================

    Analysis.cpp
    Created: 10 Feb 2017 12:12:10am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "Analysis.h"
#include "essentia/algorithmfactory.h"
#include <essentia/scheduler/network.h>

using namespace essentia;
using namespace essentia::streaming;

Analysis::Analysis() : sampleRate(44100), frameSize(2048), hopSize(1024)
{
}


Analysis::~Analysis()
{
}

void Analysis::setup()
{
}

void Analysis::newAnalysis()
{
    
    essentia::init();
    streaming::AlgorithmFactory& factory = streaming::AlgorithmFactory::instance();
    
    FileChooser chooser ("Choose audio file ...",
                         File::nonexistent,
                         "*.wav");
    
    String fileName;
    
    
    if(chooser.browseForFileToOpen())
    {
        File file(chooser.getResult());
        fileName = file.getFullPathName();
    }
    
    Algorithm* input = factory.create("MonoLoader", "filename", fileName.toStdString());
    Algorithm* fc = factory.create("FrameCutter", "frameSize", frameSize, "hopSize", hopSize);
    Algorithm* w = factory.create("Windowing", "type", "hann");
    Algorithm* spec = factory.create("Spectrum");
    
    input->output("audio") >> fc->input("signal");
    fc->output("frame") >> w->input("frame");
    w->output("frame") >> spec->input("frame");
    spec->output("spectrum") >> NOWHERE;
    
    essentia::scheduler::Network::Network n(input);
    
    n.run();
    n.clear();
    
    essentia::shutdown();
}
