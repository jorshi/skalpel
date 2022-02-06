/*
  ==============================================================================

    AnalysisMrs.cpp
    Created: 10 Feb 2017 10:05:14am
    Author:  Jordie Shier 

  ==============================================================================
*/

#include "AnalysisMrs.h"


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


SineModel::Ptr AnalysisMrs::runAnalysis()
{
    const String fileName = audioFile.getFullPathName();
    SineModel::Ptr model = new SineModel;
    
    ScopedPointer<AudioFormatReader> reader = fileLoader_.getAudioReader(audioFile);
    
    peakDetection(model, fileName, reader);
    sineTracking(model);
    cleanModel(model);
    
    return model;
}


/** 
 *  Peak Detection
 *  Opens an audio file and runs peak detection, filling a
 *  SineModel object with a sinusoidal model
 */
void AnalysisMrs::peakDetection(SineModel::Ptr sineModel, String filename, AudioFormatReader* reader)
{
    // User defined parameters
    int hopSize = params_.getHopSize();
    int frameSize = params_.getFrameSize();
    float thresh;
    if (!params_.getRawValue("analysis_amp_thresh", thresh))
        thresh = -80.0;

    // Noise Floor
    const double noiseFloor = 1e-14;
    
    float sampleRate = reader->sampleRate;
    AudioBuffer<float> inputBuffer(1, frameSize);
    
    // Complex vectors for frequency domain calculations
    std::vector<dsp::Complex<float>> timeDomain(frameSize);
    std::vector<dsp::Complex<float>> spectral(frameSize);
    
    // Forward FFT class
    dsp::FFT forwardFFT(std::log2(frameSize));
    
    // Create and normalize a Hamming window
    std::vector<double> window(frameSize);
    SynthUtils::windowingFillHamming(window);
    auto sum = std::accumulate(window.begin(), window.end(), 0.0);
    std::for_each(window.begin(), window.end(), [sum](auto &c){ c /= sum; });
  
    // Input samples from buffer
    float* inputSamples = inputBuffer.getWritePointer(0);
    inputBuffer.clear();
    
    // First half of frame is padded with zeros
    reader->read(&inputBuffer, frameSize/2, frameSize/2, 0, true, false);
    int readPtr = frameSize/2;
    
    // Run audio processing loop
    while(readPtr < reader->lengthInSamples)
    {
        // Store input as a complex number for FFT - Do Zero Phasing and apply window
        for (int i = 0; i < frameSize; ++i)
        {
            timeDomain.at(i).real(inputSamples[(i + (int)(frameSize/2)) % frameSize] *
                window[(i + (int)(frameSize/2)) % frameSize]);
        }
        
        // Perform forward FFT
        forwardFFT.perform(timeDomain.data(), spectral.data(), false);
        
        std::vector<double> phases((int)(frameSize/2));
        
        double mag = 0.0;
        double pMag = 0.0;
        double ppMag = 0.0;
        
        // New frame for storing calculated peaks
        std::vector<SineElement> frameElements;

        int i = 0;
        for (auto bin = spectral.begin(); bin != (spectral.end() - (int)frameSize/2); ++bin, ++i)
        {
            mag = 20*std::log10(std::sqrt(bin->real()*bin->real() + bin->imag()*bin->imag()));
            
            // Before calculating phase, clear noise
            if (std::abs(bin->real()) < noiseFloor) { bin->real(0.0); }
            if (std::abs(bin->imag()) < noiseFloor) { bin->imag(0.0); }
            
            // Calclute phase and then unwrap
            phases[i] = std::atan2(bin->imag(), bin->real());
            
            // Check if the previous bin had a local maxima
            if (i > 0 && ppMag < pMag && mag < pMag)
            {
                if (pMag > thresh)
                {
                    /* Peak interpolation using Parbolic Interpolation */
                    
                    // Find the interpolated location, in terms of bins
                    double ipLoc = (i-1) + (0.5*(ppMag-mag)/(ppMag - 2*pMag + mag));
                    
                    // Convert that location to frequency
                    double ipFreq = ipLoc * sampleRate / frameSize;
                    
                    // Interpolate amplitude
                    double ipAmp = pMag - 0.25*(ppMag-mag)*(ipLoc-(i-1));
                    
                    // Phase Interpolation
                    int closestBin = std::round(ipLoc);
                    double factor = ipLoc - closestBin;
                    int ipPhaseBin = (factor < 0) ? closestBin - 1 : closestBin + 1;
                    double ipPhase;
                    
                    // Only interpolate if there is not a phase jump between bins
                    auto PI = MathConstants<double>::pi;
                    if (std::abs(phases[ipPhaseBin] - phases[closestBin]) < PI)
                    {
                        ipPhase = factor * phases[ipPhaseBin] + (1-factor)*phases[closestBin];
                    }
                    else
                    {
                        ipPhase = phases[closestBin];
                    }
                    
                    // Save all the detected peaks
                    frameElements.emplace_back(ipFreq, ipAmp, ipPhase);
                }
                
            }
            
            // Save magnitudes for peak picking
            ppMag = pMag;
            pMag = mag;
        }
        
        // Shift samples by a hop size
        for (int j = hopSize; j < frameSize; j++)
        {
            inputSamples[j - hopSize] = inputSamples[j];
        }
        reader->read(&inputBuffer, frameSize - hopSize, hopSize, readPtr, true, false);
        readPtr += hopSize;

        sineModel->addFrame(frameElements);
    }
    
    sineModel->setSampleRate(sampleRate);
    sineModel->setFrameSize(frameSize);
    sineModel->setHopSize(hopSize);
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
    
    int trackId = 0;
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
                std::vector<double> diff(trackIndexes.size());
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
            newSines.back().setNewTrack(trackId++);
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
    
    double frameLength = sineModel->getFrameSize()/sineModel->getSampleRate();
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
