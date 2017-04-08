/*
  ==============================================================================

    SynthesisUtils.cpp
    Created: 15 Feb 2017 11:06:15pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SynthesisUtils.h"



namespace SynthUtils {
    
    
    void windowingFillBlackmanHarris(realvec& envelope)
    {
        mrs_natural N = envelope.getSize();
        mrs_real a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
        for (mrs_natural t = 0; t < N; t++)
        {
            envelope(t) = a0  - a1 * cos(2.0 * PI * t / (N - 1.0))
            + a2 * cos(4.0 * PI * t / (N - 1.0))
            - a3 * cos(6.0 * PI * t / (N - 1.0));
        }
    }
    
    
    void windowingFillTriangle(realvec& envelope)
    {
        mrs_natural N = envelope.getSize();
        for (mrs_natural t = 0; t < N; t++)
        {
            envelope(t) = 2.0/N * (N/2.0 - std::abs(t - (N - 1.0)/2.0));
        }
    }
    
    void windowingFillRaisedCosine(realvec& envelope, mrs_real alpha, mrs_real beta)
    {
        mrs_natural N = envelope.getSize();
        for (mrs_natural t = 0; t < N; t++)
        {
            envelope(t) = alpha - beta * cos(2.0 * PI * t / (N - 1.0));
        }
    }
    
    void windowingFillHamming(realvec& envelope)
    {
        windowingFillRaisedCosine(envelope, 0.54, 0.46);
    }
    
    void createSynthesisWindow(realvec& envelope, mrs_natural hopSize)
    {
        // Create a normalized blackman harris window
        windowingFillBlackmanHarris(envelope);
        envelope /= envelope.sum();
        
        // Create a triangle window
        mrs_realvec triangle(hopSize*2);
        windowingFillTriangle(triangle);
        
        // Create a triangle window in the middle of a window twice it's size,
        // divided by a blackman harris window
        for (int i = 0; i < envelope.getSize(); ++i)
        {
            if (i >= hopSize && i < hopSize*3)
            {
                envelope(i) = triangle(i-hopSize)/envelope(i);
            }
            else
            {
                envelope(i) = 0.0;
            }
        }
    }
}

