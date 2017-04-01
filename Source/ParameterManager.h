/*
  ==============================================================================

    ParameterManager.h
    Created: 18 Mar 2017 8:49:41pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef ParameterManager_H_INCLUDED
#define ParameterManager_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>

class ParameterManager
{
public:
    
    // Default Constructor
    ParameterManager(int num, AudioProcessorValueTreeState* p);
    
    virtual ~ParameterManager();

    // Pure virtual method to create parameters
    virtual void createParameters()=0;
    
    // Map to parameters that are associated to the manager
    String getParamId(const String paramName) const;
    
    AudioProcessorValueTreeState* getParameters() { return parameters_; };

protected:
    const int soundNum_;
    std::map<String, String> paramMap_;
    AudioProcessorValueTreeState* parameters_;
    
private:

};



#endif  // ParameterManager_H_INCLUDED
