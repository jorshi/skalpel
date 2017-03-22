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
#include "SoundInterface.h"

//==============================================================================
/*
*/
class LoomFileDrop    : public Component,
                        public FileDragAndDropTarget,
                        public ActionListener
{
public:
    LoomFileDrop(ActionListener* parent, SoundInterface& s);
    ~LoomFileDrop();

    void paint (Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag(const StringArray& files) override;
    void filesDropped(const StringArray& files, int x, int y) override;

private:
    
    // Get called when a child sends a message: allows messages to bubble up
    void actionListenerCallback(const String& message) override;
    
    ActionBroadcaster broadcaster;
    
    Colour buttonEdgeColour;
    
    SoundInterface& soundInterface;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoomFileDrop)
};


#endif  // LOOMFILEDROP_H_INCLUDED
