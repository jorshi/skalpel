/*
  ==============================================================================

    LoomFileDrop.h
    Created: 21 Mar 2017 11:04:08pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#ifndef LOOMFILEDROP_H_INCLUDED
#define LOOMFILEDROP_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class LoomFileDrop    : public Component,
                        public FileDragAndDropTarget
{
public:
    LoomFileDrop();
    ~LoomFileDrop();

    void paint (Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:
    Colour buttonEdgeColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomFileDrop)
};


#endif  // LOOMFILEDROP_H_INCLUDED
