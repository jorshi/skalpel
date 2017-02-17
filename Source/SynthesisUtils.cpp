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
    
    void createSynthesisWindow(realvec& envelope)
    {
        windowingFillBlackmanHarris(envelope);
        
        // Create a triangle window
        mrs_realvec triangle;
        triangle.create(envelope.getSize());
        windowingFillTriangle(triangle);
        
        mrs_real bhSum(0.0);
        
        // Calculate sum of the blackman harris window
        for (int i = 0; i < envelope.getSize(); ++i)
        {
            bhSum += envelope(i);
        }
        
        // Normalize blackman harris and multiply with triangle
        for (int i = 0; i < envelope.getSize(); ++i)
        {
            envelope(i) = envelope(i)/bhSum;
            envelope(i) = envelope(i)*triangle(i);
        }
    }
}

