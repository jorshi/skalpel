/*
  ==============================================================================

    FileLoader.cpp
    Created: 17 Mar 2017 1:20:56pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "FileLoader.h"

FileLoader::FileLoader()
{
    formatManager.registerFormat(new WavAudioFormat, true);
}

FileLoader::~FileLoader()
{
    
}


File* FileLoader::getNewAudioFile()
{
    // Get new audio file
    FileChooser chooser ("Choose audio file ...",
                         File::nonexistent,
                         "*.wav");
    
    // Open file and make sure it is a valid audio file
    // TODO: keep the reader here for the analysis phase as opposed to
    // loading the audio file and then turning around and doing the work
    // again??
    if(chooser.browseForFileToOpen())
    {
        File* file = new File(chooser.getResult());
        ScopedPointer<AudioFormatReader> reader = formatManager.createReaderFor(*file);
        
        if (reader != nullptr)
        {
            return file;
        }
    }
    
    
    return nullptr;
}
