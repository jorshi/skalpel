/*
  ==============================================================================

    SynthesisUtils.cpp
    Created: 15 Feb 2017 11:06:15pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SynthesisUtils.h"



namespace SynthUtils {
    
    
    void windowingFillBlackmanHarris(std::vector<double>& envelope)
    {
        auto PI = MathConstants<double>::pi;
        size_t N = envelope.size();
        double a0 = 0.35875, a1 = 0.48829, a2 = 0.14128, a3 = 0.01168;
        for (size_t t = 0; t < N; t++)
        {
            envelope[t] = a0  - a1 * cos(2.0 * PI * t / (N - 1.0))
            + a2 * cos(4.0 * PI * t / (N - 1.0))
            - a3 * cos(6.0 * PI * t / (N - 1.0));
        }
    }
    
    
    void windowingFillTriangle(std::vector<double>& envelope)
    {
        size_t N = envelope.size();
        for (size_t t = 0; t < N; t++)
        {
            envelope[t] = 2.0/N * (N/2.0 - std::abs(t - (N - 1.0)/2.0));
        }
    }
    
    void windowingFillRaisedCosine(std::vector<double>& envelope, double alpha, double beta)
    {
        auto PI = MathConstants<double>::pi;
        size_t N = envelope.size();
        for (size_t t = 0; t < N; t++)
        {
            envelope[t] = alpha - beta * cos(2.0 * PI * t / (N - 1.0));
        }
    }
    
    void windowingFillHamming(std::vector<double>& envelope)
    {
        windowingFillRaisedCosine(envelope, 0.54, 0.46);
    }
    
    void createSynthesisWindow(std::vector<double>& envelope, int hopSize)
    {
        // Create a normalized blackman harris window
        windowingFillBlackmanHarris(envelope);
        auto sum = std::accumulate(envelope.begin(), envelope.end(), 0.0);
        std::for_each(envelope.begin(), envelope.end(), [sum](double &c){ c /= sum; });
        
        // Create a triangle window
        std::vector<double> triangle(hopSize*2);
        windowingFillTriangle(triangle);
        
        // Create a triangle window in the middle of a window twice it's size,
        // divided by a blackman harris window
        for (int i = 0; i < envelope.size(); ++i)
        {
            if (i >= hopSize && i < hopSize*3)
            {
                envelope[i] = triangle[i-hopSize]/envelope[i];
            }
            else
            {
                envelope[i] = 0.0;
            }
        }
    }
}

