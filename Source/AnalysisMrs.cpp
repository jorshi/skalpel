/*
  ==============================================================================

    AnalysisMrs.cpp
    Created: 10 Feb 2017 10:05:14am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisMrs.h"


using namespace Marsyas;

// Default Constructor
AnalysisMrs::AnalysisMrs(AnalysisParameterManager& params) : params_(params)
{
}

// Construtor with audio file passed in
AnalysisMrs::AnalysisMrs(File input, AnalysisParameterManager& params) :
    audioFile(input), params_(params)
{
}

// Deconstructor
AnalysisMrs::~AnalysisMrs() {}


// Get a new wave file and run sinusoidal analysis
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
    analysisModel_ = new SineModel();
    peakDetection(analysisModel_, fileName);
    sineTracking(analysisModel_);
    cleanModel(analysisModel_);
}

SineModel* AnalysisMrs::runAnalysis()
{
    const String fileName = audioFile.getFullPathName();
    SineModel::Ptr model = new SineModel;
    
    peakDetection(model, fileName);
    sineTracking(model);
    cleanModel(model);
    
    return model;
}


/** 
 *  Peak Detection
 *  Opens an audio file and runs peak detection, filling a
 *  SineModel object with a sinusoidal model
 */
void AnalysisMrs::peakDetection(SineModel::Ptr sineModel, String filename)
{
    AudioProcessorValueTreeState* parameters = params_.getParameters();
    
    Value val = parameters->getParameterAsValue(params_.getParamId("analysis_window"));
    
    
    // Noise Floor
    const mrs_real noiseFloor = 1e-14;
    
    // User defined parameters
    int hopSize = params_.getHopSize();
    int frameSize = params_.getFrameSize();
    float thresh;
    if (!params_.getRawValue("analysis_amp_thresh", thresh))
        thresh = -80.0;
    
    mrs_real sampleRate;
    
    // Complex vectors for frequency domain calculations
    std::vector<FFT::Complex> timeDomain(frameSize);
    std::vector<FFT::Complex> spectral(frameSize);
    
    // Forward FFT class
    FFT forwardFFT(std::log2(frameSize), false);

    // Create a Marsyas system
    MarSystemManager mng;
    ScopedPointer<MarSystem> network = mng.create("Series/analysis");
    
    network->addMarSystem(mng.create("SoundFileSource/input"));
    network->addMarSystem(mng.create("MixToMono/mono"));
    network->addMarSystem(mng.create("ShiftInput/shift"));
    network->addMarSystem(mng.create("Gain/gain"));
    
    network->updControl("mrs_natural/inSamples", hopSize);
    network->updControl("SoundFileSource/input/mrs_string/filename", filename.toStdString());
    network->updControl("ShiftInput/shift/mrs_natural/winSize", frameSize);
    
    // Get pointer to a frame of shifted samples from input
    MarControlPtr shifted = network->getControl("ShiftInput/shift/mrs_realvec/processedData");
    
    // Create and normalize a Hamming window
    mrs_realvec window(frameSize);
    SynthUtils::windowingFillHamming(window);
    window /= window.sum();
  
    // Run audio processing loop
    while(network->getControl("SoundFileSource/input/mrs_bool/hasData")->to_bool())
    {
        network->tick();
        
        // Update sample rate -- shouldn't change but we need to do it in the loop
        sampleRate = network->getControl("SoundFileSource/input/mrs_real/osrate")->to_real();

        // Shifted frame
        mrs_realvec input = shifted->to_realvec();
        
        // Store input as a complex number for FFT - Do Zero Phasing and apply window
        for (int i = 0; i < frameSize; ++i)
        {
            timeDomain.at(i).r = input((i + (int)(frameSize/2)) % frameSize) *
                window((i + (int)(frameSize/2)) % frameSize);
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data());
        
        
        mrs_realvec phases((int)(frameSize/2));
        
        mrs_real mag = 0.0;
        mrs_real pMag = 0.0;
        mrs_real ppMag = 0.0;
        
        // New frame for storing calculated peaks
        std::vector<SineElement> frameElements;

        int i = 0;
        for (auto bin = spectral.begin(); bin != (spectral.end() - (int)frameSize/2); ++bin, ++i)
        {
            mag = 20*std::log10(std::sqrt(bin->r*bin->r + bin->i*bin->i));
            
            // Before calculating phase, clear noise
            if (std::abs(bin->r) < noiseFloor) { bin->r = 0.0; }
            if (std::abs(bin->i) < noiseFloor) { bin->i = 0.0; }
            
            // Calclute phase and then unwrap
            phases(i) = std::atan2(bin->i, bin->r);
            
            // Check if the previous bin had a local maxima
            if (i > 0 && ppMag < pMag && mag < pMag)
            {
                if (pMag > thresh)
                {
                    /* Peak interpolation using Parbolic Interpolation */
                    
                    // Find the interpolated location, in terms of bins
                    mrs_real ipLoc = (i-1) + (0.5*(ppMag-mag)/(ppMag - 2*pMag + mag));
                    
                    // Convert that location to frequency
                    mrs_real ipFreq = ipLoc * sampleRate / frameSize;
                    
                    // Interpolate amplitude
                    mrs_real ipAmp = pMag - 0.25*(ppMag-mag)*(ipLoc-(i-1));
                    
                    // Phase Interpolation
                    mrs_natural closestBin = std::round(ipLoc);
                    mrs_real factor = ipLoc - closestBin;
                    mrs_natural ipPhaseBin = (factor < 0) ? closestBin - 1 : closestBin + 1;
                    mrs_real ipPhase;
                    
                    // Only interpolate if there is not a phase jump between bins
                    if (std::abs(phases(ipPhaseBin) - phases(closestBin)) < PI)
                    {
                        ipPhase = factor * phases(ipPhaseBin) + (1-factor)*phases(closestBin);
                    }
                    else
                    {
                        ipPhase = phases(closestBin);
                    }
                    
                    // Save all the detected peaks
                    frameElements.emplace_back(ipFreq, ipAmp, ipPhase);
                }
                
            }
            
            // Save magnitudes for peak picking
            ppMag = pMag;
            pMag = mag;
        }

        sineModel->addFrame(frameElements);
    }
    
    sineModel->setSampleRate(sampleRate);
    sineModel->setFrameSize(hopSize);
}


void AnalysisMrs::sineTracking(SineModel::Ptr sineModel)
{
    // Get user defined parameters
    float freqDevOffset;
    if (!params_.getRawValue("analysis_freq_offset", freqDevOffset))
        freqDevOffset = 20.0;
    
    float freqDevSlope;
    if (!params_.getRawValue("analysis_freq_slop", freqDevSlope))
        freqDevSlope = 0.001;
    
    mrs_natural trackId = 0;
    std::vector<std::vector<SineElement>> tracks;
    
    // Iterate through elements of the sine model
    for (auto frame = sineModel->begin(); frame != sineModel->end(); ++frame)
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

        jassert(newSines.size() == frame->size());
        
        tracks.push_back(newSines);
    }
    
    sineModel->setSineModel(tracks);
}


void AnalysisMrs::cleanModel(SineModel::Ptr sineModel)
{
    // User Defined Parameters
    float minDuration;
    if (!params_.getRawValue("analysis_duration", minDuration))
        minDuration = 20;
    
    float numSines;
    if(!params_.getRawValue("analysis_sines", numSines))
        numSines = 150;
    
    mrs_real frameLength = sineModel->getFrameSize()/sineModel->getSampleRate();
    int minFrames = (int)std::ceil((minDuration/1000)/frameLength);
    
    std::vector<int> trackLengths;
    std::vector<std::vector<SineElement*>> trackPointers;
    
    // Get a list of pointers to the elements of the sine tracks
    for (auto frame = sineModel->begin(); frame != sineModel->end(); ++frame)
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
    for (auto frame = sineModel->begin(); frame != sineModel->end(); ++frame)
    {
        auto newEnd = std::remove_if(frame->begin(), frame->end(), [&](SineElement& a) {
            return (a.getTrack() == -1);
        });
        
        frame->resize(std::distance(frame->begin(), newEnd));
        
        // If there are still too many sines in the frame, then remove based on amplitude
        if (frame->size() > numSines)
        {
            std::sort(frame->begin(), frame->end(), [&](SineElement& a, SineElement& b) {
                return (a.getAmp() > b.getAmp());
            });
            frame->resize(numSines);
        }
    }
}
