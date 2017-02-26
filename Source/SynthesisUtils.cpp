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
    
    void createSynthesisWindow(realvec& envelope, mrs_natural hopSize)
    {
        windowingFillBlackmanHarris(envelope);
        
        // Create a triangle window
        mrs_realvec triangle;
        triangle.create(hopSize*2);
        windowingFillTriangle(triangle);
        
        mrs_realvec fullWindow;
        fullWindow.create(envelope.getSize());
        
        for (int i = 0; i < triangle.getSize(); ++i)
        {
            fullWindow(i + hopSize) = triangle(i);
        }
        
        mrs_real bhSum = envelope.sum();
        
        // Normalize blackman harris and multiply with triangle
        // TODO: this loop can be merged with above loop
        for (int i = 0; i < envelope.getSize(); ++i)
        {
            envelope(i) = fullWindow(i)/(envelope(i)/bhSum);
        }
    }
}

