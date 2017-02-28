/*
  ==============================================================================

    AnalysisMrs.cpp
    Created: 10 Feb 2017 10:05:14am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisMrs.h"


using namespace Marsyas;

AnalysisMrs::AnalysisMrs() {}

AnalysisMrs::~AnalysisMrs() {}

void AnalysisMrs::newAnalysis()
{
    // Get new audio file
    FileChooser chooser ("Choose audio file ...",
                         File::nonexistent,
                         "*.wav");
    
    String fileName;
    if(chooser.browseForFileToOpen())
    {
        File file(chooser.getResult());
        fileName = file.getFullPathName();
    }
    else
    {
        return;
    }
    
    // Run sineusoidal analysis
    _analysisModel = SineModel();
    peakDetection(_analysisModel, fileName);
    sineTracking(_analysisModel);
    cleanModel(_analysisModel);
    
}


void AnalysisMrs::peakDetection(SineModel& sineModel, String filename)
{
    MarSystemManager mng;
    
    const mrs_real noiseFloor = 1e-14;
    
    int hopSize = 128;
    int frameSize = 2048;
    
    // Complex vectors for frequency domain calculations
    std::vector<FFT::Complex> timeDomain(frameSize);
    std::vector<FFT::Complex> spectral(frameSize);
    
    // Forward FFT class
    FFT forwardFFT(std::log2(frameSize), false);
    
    mrs_real sampleRate;
    
    // This is redundant, but if we want to experiment with differnt amounts
    // of zero padding this will be helpful
    int zeroPaddedWindow = frameSize*2;
    int windowSize = zeroPaddedWindow/2;
    
    mrs_real thresh = -80.0;
    
    MarSystem* network = mng.create("Series/analysis");
    
    network->addMarSystem(mng.create("SoundFileSource/input"));
    network->addMarSystem(mng.create("MixToMono/mono"));
    network->addMarSystem(mng.create("ShiftInput/shift"));
    network->addMarSystem(mng.create("Windowing/window"));
    network->addMarSystem(mng.create("Gain/gain"));
    
    network->updControl("mrs_natural/inSamples", hopSize);
    network->updControl("SoundFileSource/input/mrs_string/filename", filename.toStdString());
    network->updControl("ShiftInput/shift/mrs_natural/winSize", frameSize);
    network->updControl("Windowing/window/mrs_string/type", "Hamming");
    network->updControl("Windowing/window/mrs_bool/zeroPhasing", true);
    network->updControl("Windowing/window/mrs_bool/normalize", true);
    
    MarControlPtr frame = network->getControl("Windowing/window/mrs_realvec/processedData");
    
    // Create and normalize a Hamming window
    mrs_realvec window(frameSize);
    SynthUtils::windowingFillHamming(window);
    
    while(network->getControl("SoundFileSource/input/mrs_bool/hasData")->to_bool())
    {
        // Update sample rate -- shouldn't change but we need to do it in the loop
        sampleRate = network->getControl("SoundFileSource/input/mrs_real/osrate")->to_real();
        
        // Get spectrum at this frame
        mrs_realvec data = frame->to_realvec();
        
        // Store input as a complex number for FFT
        for (int i = 0; i < frameSize; ++i)
        {
            timeDomain.at(i).r = data(i);
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data());
        
        mrs_realvec magnitudes((int)(frameSize/2));
        mrs_realvec phases((int)(frameSize/2));
        std::vector<mrs_real> peaks;
        
        mrs_real mag = 0.0;
        mrs_real pMag = 0.0;
        mrs_real ppMag = 0.0;

        mrs_real phaseDiff = 0.0;
        mrs_real phaseOffset = 0.0;
        
        std::vector<SineElement> frameElements;
        
        int i = 0;
        for (auto bin = spectral.begin(); bin != (spectral.end() - (int)frameSize/2); ++bin, ++i)
        {
            mag = 20*std::log10(std::sqrt(bin->r*bin->r + bin->i*bin->i));
            
            // Before calculating phase, clear noise
            if (std::abs(bin->r) < noiseFloor) {bin->r = 0.0;}
            if (std::abs(bin->i) < noiseFloor) {bin->i = 0.0;}
            
            // Calclute phase and then unwrap
            phases(i) = std::atan2(bin->i, bin->r);
            
            // Unwrap phase
            if (i > 0)
            {
                phaseDiff = (phases(i) + phaseOffset) - phases(i-1);
                if (phaseDiff > PI)
                {
                    phases(i) += (phaseOffset -= TWOPI);
                }
                else if (phaseDiff < -PI)
                {
                    phases(i) += (phaseOffset += TWOPI);
                }
                else
                {
                    phases(i) += phaseOffset;
                }
            }
            
            // Check if the previous bin had a local maxima
            if (ppMag < pMag && mag < pMag)
            {
                if (pMag > thresh)
                {
                    peaks.push_back(i-1);
                    
                    // Peak interpolation using Parbolic Interpolation //
                    
                    // Find the interpolated location, in terms of bins
                    mrs_real ipLoc = (i-1) + (0.5*(ppMag-mag)/(ppMag - 2*pMag + mag));
                    
                    // Convert that location to frequency
                    mrs_real ipFreq = ipLoc * sampleRate / frameSize;
                    
                    // Interpolate amplitude
                    mrs_real ipAmp = pMag - 0.25*(ppMag-mag)*(ipLoc-(i-1));
                    
                    // Check if linear interpolation should occur between last two bins, or this
                    // bin and the last bin
                    mrs_natural phaseIndex = 1;
                    if (ipLoc < (i-1))
                        phaseIndex = 2;
                    
                    // Linear interpolation on phase
                    mrs_real ipPhase = phases(i-phaseIndex ) +
                        (ipLoc - (i-phaseIndex))*(phases(i-phaseIndex-1) - phases(i-phaseIndex));
                    
                    // Save all the detected peaks
                    frameElements.emplace_back(ipFreq, ipAmp, ipPhase);
                }
                
            }
            
            // Save magnitudes for peak picking
            ppMag = pMag;
            pMag = mag;
        }

        sineModel.addFrame(frameElements);
        network->tick();
    }
    
    sineModel.setSampleRate(sampleRate);
    sineModel.setFrameSize(hopSize);
    
    delete network;
}


void AnalysisMrs::sineTracking(SineModel& sineModel)
{
    mrs_real freqDevOffset = 20.0;
    mrs_real freqDevSlope = 0.01;
    mrs_natural trackId = 0;
    
    std::vector<std::vector<SineElement>> tracks;
    
    // Iterate through elements of the sine model
    for (auto frame = sineModel.begin(); frame != sineModel.end(); ++frame)
    {
        // Sine elements to place for this frame
        std::vector<SineElement> newSines;
        
        std::vector<int> indexesToRemove;
        
        // If this frame is empty then move to next one
        if (frame->size() <= 0)
        {
            tracks.push_back(newSines);
            continue;
        }
        
        // Create an array of indexes corresponding to sorted magnitude
        // of current peaks in frame, starting with highest magnitude first
        std::size_t n(0);
        std::vector<int> indexes(frame->size());
        std::generate(indexes.begin(), indexes.end(), [&]{ return n++; });
        std::sort(indexes.begin(), indexes.end(), [&](int i1, int i2) {
            return (frame->at(i1).getAmp() > frame->at(i2).getAmp());
        });

        // Check if there were any tracks from last iteration that
        // can be continued
        if (tracks.size() > 0 && tracks.back().size() > 0)
        {
            n = 0;
            std::vector<int> trackIndexes(tracks.back().size());
            std::generate(trackIndexes.begin(), trackIndexes.end(), [&]{ return n++; });
            
            for (auto index = indexes.begin(); index != indexes.end(); ++index)
            {
                // Check to see if all tracks have been assigned
                if (trackIndexes.size() == 0)
                    break;
                
                // Find the track closest to the frequency of incoming bin
                std::vector<mrs_real> diff(trackIndexes.size());
                std::transform(trackIndexes.begin(), trackIndexes.end(), diff.begin(), [&](int a) {
                    return std::abs(tracks.back().at(a).getFreq() - frame->at(*index).getFreq());
                });
                
                // Index of the closest track from the previous frame of tracks to the current sine of this frame
                auto minDiff = std::min_element(diff.begin(), diff.end());
                const int track = trackIndexes.at(std::distance(diff.begin(), minDiff));
                
                // If the difference in frequency from the determined track is within the acceptable
                // deviation amount, then continue this track
                if (*minDiff < (freqDevOffset + freqDevSlope * frame->at(*index).getFreq()))
                {
                    // Save this sine element with the track index from the previous frame
                    newSines.emplace_back(frame->at(*index));
                    newSines.back().setTrack(tracks.back().at(track).getTrack());
                    
                    // Remove index from tracks to assign
                    trackIndexes.erase(std::find(trackIndexes.begin(), trackIndexes.end(), track));
                    
                    // Save this index number to remove from indexes vector after track continuation
                    indexesToRemove.push_back(*index);
                }
            }
        }
        
        // Remove indexes corresponding to peaks that were used in track continuation
        for (auto i = indexesToRemove.begin(); i != indexesToRemove.end(); ++i)
        {
            indexes.erase(std::find(indexes.begin(), indexes.end(), *i));
        }
        
        // Save remaining peaks from the current frame into the sine vector
        for (auto i = indexes.begin(); i != indexes.end(); ++i)
        {
            newSines.emplace_back(frame->at(*i));
            newSines.back().setTrack(trackId++);
        }

        assert(newSines.size() == frame->size());
        
        tracks.push_back(newSines);
    }
    
    sineModel.setSineModel(tracks);
}


void AnalysisMrs::cleanModel(SineModel& sineModel)
{
    
    // Min duration in seconds
    mrs_real minDuration = 0.02;
    mrs_real frameLength = sineModel.getFrameSize()/sineModel.getSampleRate();
    int minFrames = (int)std::ceil(minDuration/frameLength);
    
    std::vector<int> trackLengths;
    std::vector<std::vector<SineElement*>> trackPointers;
    
    // Get a list of pointers to the elements of the sine tracks
    for (auto frame = sineModel.begin(); frame != sineModel.end(); ++frame)
    {
        for (auto sine = frame->begin(); sine != frame->end(); ++sine)
        {
            if (sine->getTrack()+1 > trackPointers.size())
            {
                trackPointers.resize(sine->getTrack()+1);
            }
            trackPointers.at(sine->getTrack()).push_back(&(*sine));
        }
    }
    
    // Mark each sine element that should be deleted
    int tracksDeleted = 0;
    for (auto track = trackPointers.begin(); track != trackPointers.end(); ++track)
    {
        if (track->size() < minFrames)
        {
            for (auto sine = track->begin(); sine != track->end(); ++sine)
            {
                (*sine)->setTrack(-1);
            }
            ++tracksDeleted;
        }
        else if (tracksDeleted > 0)
        {
            for (auto sine = track->begin(); sine != track->end(); ++sine)
            {
                (*sine)->setTrack((*sine)->getTrack() - tracksDeleted);
            }
        }
    }
    
    // Clear out all elements that didn't make the cut
    for (auto frame = sineModel.begin(); frame != sineModel.end(); ++frame)
    {
        auto newEnd = std::remove_if(frame->begin(), frame->end(), [&](SineElement& a) {
            return (a.getTrack() == -1);
        });
        
        frame->resize(std::distance(frame->begin(), newEnd));
    }
}
