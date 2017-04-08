/*
  ==============================================================================

    StochasticModel.cpp
    Created: 7 Apr 2017 6:56:13pm
    Author:  Jordie Shier

  ==============================================================================
*/

#include "StochasticModel.h"

void StochasticModel::addFrame(const std::vector<FFT::Complex> &spectrum)
{
    jassert(spectrum.size() >= factor_);
    model_.emplace_back(factor_);
    for (int i = 0; i < factor_; i++)
    {
        model_.back()[i] = spectrum[i].r;
    }
}
