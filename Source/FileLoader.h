/*
  ==============================================================================

    FileLoader.h
    Created: 17 Mar 2017 1:20:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef FILELOADER_H_INCLUDED
#define FILELOADER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class FileLoader
{
public:
    
    // Constructor
    FileLoader();
    
    // Destructor
    ~FileLoader();
    
    // Loads a new audio file
    File* getNewAudioFile();
    
private:
    
    AudioFormatManager formatManager;
};


#endif  // FILELOADER_H_INCLUDED
