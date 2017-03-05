/*
  ==============================================================================

    SineElement.cpp
    Created: 11 Feb 2017 2:09:40pm
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "SineElement.h"
using boost::property_tree::ptree;


// Default Constructor
SineElement::SineElement() : _freq(0.0), _amp(0.0), _phase(0.0), _track(-1)
{

}

SineElement::SineElement(mrs_real freq, mrs_real amp, mrs_real phase) :
_freq(freq), _amp(amp), _phase(phase), _track(-1)
{
    
}

SineElement::~SineElement()
{
    
}

//==============================================================================
// SineModel

SineModel::SineModel() : _sampleRate(0.0), _frameSize(0)
{
    _sineModel = SineModel::SineMatrix();
}

SineModel::~SineModel()
{
}

void SineModel::addFrame(std::vector<SineElement> newFrame)
{
    _sineModel.emplace_back(newFrame);
}

SineModel getTestModel()
{
    mrs_natural hopSize = 128;
    mrs_real rate = 44100.0;
    
    SineModel testModel;
    testModel.setHopSize(hopSize);
    testModel.setFrameSize(hopSize);
    testModel.setSampleRate(rate);
    
    mrs_real freq = 440.0;
    mrs_real phase = 0.0;
    mrs_real phaseInc = (hopSize/rate)*freq*2*PI;
    
    
    
    for (int i = 0; i < 100; ++i)
    {
        SineElement newElement(440.0, -12.0, phase);
        phase += phaseInc;
        SineModel::SineFrame newFrame;
        newFrame.push_back(newElement);
        testModel.addFrame(newFrame);
    }
    
    return testModel;
}

SineModel getSawModel()
{
    mrs_natural hopSize = 128;
    mrs_real rate = 44100.0;
    
    SineModel testModel;
    testModel.setHopSize(hopSize);
    testModel.setFrameSize(hopSize);
    testModel.setSampleRate(rate);
    
    ptree pt1;
    read_xml("/Users/jshier/Development/Libraries/sms-tools/software/models_interface/output.xml", pt1);
    
    
    //BOOST_FOREACH(ptree::value_type const& v, pt.get_child("frame"))
    //{
    //    SineModel::SineFrame newFrame;
    //    testModel.addFrame(newFrame);
    //}
    // Traverse property tree example
    BOOST_FOREACH( boost::property_tree::ptree::value_type const& node, pt1.get_child( "model.frames" ) )
    {
        boost::property_tree::ptree subtree = node.second;
        SineModel::SineFrame newFrame;
        
        if( node.first == "frame" )
        {
            BOOST_FOREACH( boost::property_tree::ptree::value_type const& v, subtree.get_child( "sines" ) )
            {
                boost::property_tree::ptree subsubtree = v.second;

                SineElement newSine;
                if (v.first == "sine")
                {
                    BOOST_FOREACH( boost::property_tree::ptree::value_type const& t, subsubtree.get_child( "" ) )
                    {

                        auto label = t.first;
                        if (label == "freq")
                        {
                            std::string value = subsubtree.get<std::string>( label );
                            newSine.setFreq(std::stod(value));
                        }
                        else if (label == "amp")
                        {
                            std::string value = subsubtree.get<std::string>( label );
                            newSine.setAmp(std::stod(value));
                        }
                        else if (label == "phase")
                        {
                            std::string value = subsubtree.get<std::string>( label );
                            newSine.setPhase(std::stod(value));
                        }
                        else if (label == "track")
                        {
                            std::string value = subsubtree.get<std::string>( label );
                            newSine.setTrack(std::stod(value));
                        }
                    }
                }
                
                newFrame.push_back(newSine);

            }
            testModel.addFrame(newFrame);
        }
    }
    return testModel;
}



