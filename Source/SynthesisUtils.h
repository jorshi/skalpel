/*
  ==============================================================================

    SynthesisUtils.h
    Created: 15 Feb 2017 11:06:15pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef SYNTHESISUTILS_H_INCLUDED
#define SYNTHESISUTILS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "marsyas/system/MarSystemManager.h"

using namespace Marsyas;

namespace SynthUtils {

    void windowingFillBlackmanHarris(Marsyas::realvec&);
    void windowingFillTriangle(Marsyas::realvec&);
    void createSynthesisWindow(realvec&);

}

#endif  // SYNTHESISUTILS_H_INCLUDED
