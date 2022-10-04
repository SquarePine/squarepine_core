/** */
inline void resetBuses (AudioProcessor& processor,
                        int numInputs,
                        int numOutputs)
{
    if (auto* ins = processor.getBus (false, 0))
        ins->setCurrentLayout (AudioChannelSet::canonicalChannelSet (numInputs));

    if (auto* outs = processor.getBus (true, 0))
        outs->setCurrentLayout (AudioChannelSet::canonicalChannelSet (numOutputs));

    processor.enableAllBuses();
}

//==============================================================================
/** Properly denormalises an audio buffer if denormalisation occurred.

    @see FPUFlags
*/
template<typename FloatType>
inline void zeroIfDenormalisationOccurred (juce::AudioBuffer<FloatType>& buffer)
{
    if (! FPUFlags::hasDenormalisationOccurred() || buffer.hasBeenCleared())
        return;

    auto chans = buffer.getArrayOfWritePointers();
    const auto numSamples = buffer.getNumSamples();

    for (int i = buffer.getNumChannels(); --i >= 0;)
    {
        if (auto* c = chans[i])
        {
            FloatVectorOperations::add (c, 1.0f, numSamples);
            FloatVectorOperations::add (c, -1.0f, numSamples);
        }
    }
}

//==============================================================================
/** Safely processes audio, avoiding letting a processor fuck around with a CPU's FPU flags.

    Lots of dodgy third-party plugins use 'fast' float modes, which can subtly
    screw up the audio processing pipeline and cause grotesque glitches.

    @see FPUFlags, zeroIfDenormalisationOccurred
*/
template<typename FloatType>
inline void processSafely (AudioProcessor& proc, juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages)
{
    FPUFlags::clearIfDenormalised();
    proc.processBlock (buffer, midiMessages);
    zeroIfDenormalisationOccurred (buffer);
}

//==============================================================================
template<typename FloatType>
inline void fadeOutBuffers (juce::AudioBuffer<FloatType>& buffer, MidiBuffer&)
{
    const auto numSamples = buffer.getNumSamples();

    for (int i = buffer.getNumChannels(); --i >= 0;)
        buffer.applyGainRamp (i, 0, numSamples, 1.0f, 0.0f);
}

template<typename FloatType>
inline void clearBuffers (juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();
    midiMessages.clear();
}

template<typename FloatType>
static void addFrom (juce::AudioBuffer<FloatType>& destination, juce::AudioBuffer<FloatType>& source,
                     int numChannels, int numSamples, float gainFactor = 1.0f)
{
    // NB: This first bit is for copying mono to stereo.
    if (numChannels == 1 && destination.getNumChannels() == 2)
    {
        destination.addFrom (0, 0, source.getReadPointer (0), numSamples, (FloatType) gainFactor);
        destination.addFrom (1, 0, source.getReadPointer (0), numSamples, (FloatType) gainFactor);
    }
    else
    {
        for (auto i = numChannels; --i >= 0;)
            destination.addFrom (i, 0, source.getReadPointer (i), numSamples, (FloatType) gainFactor);
    }
}

template<typename FloatType>
static void addFrom (juce::AudioBuffer<FloatType>& destination, juce::AudioBuffer<FloatType>& source, float gainFactor = 1.0f)
{
    addFrom (destination, source,
             jmin (source.getNumChannels(), destination.getNumChannels()),
             jmin (source.getNumSamples(), destination.getNumSamples()),
             gainFactor);
}

//==============================================================================
/** Quick-and-dirty function to format a timecode string. */
[[nodiscard]] inline String timeToTimecodeString (double seconds)
{
    const auto millisecs = roundToInt (seconds * 1000.0);
    const auto absMillisecs = std::abs (millisecs);

    return String::formatted ("%02d:%02d:%02d.%03d",
                                millisecs / 3600000,
                                (absMillisecs / 60000) % 60,
                                (absMillisecs / 1000)  % 60,
                                absMillisecs % 1000);
}

/** Quick-and-dirty function to format a bars/beats string. */
[[nodiscard]] inline String quarterNotePositionToBarsBeatsString (double quarterNotes, int numerator, int denominator)
{
    if (numerator == 0 || denominator == 0)
        return "1|1|000";

    const auto quarterNotesPerBar = ((double) numerator * 4.0 / (double) denominator);
    const auto beats  = (std::fmod (quarterNotes, quarterNotesPerBar) / quarterNotesPerBar) * numerator;
    const auto bar    = roundToInt (quarterNotes / quarterNotesPerBar) + 1;
    const auto beat   = roundToInt (beats) + 1;
    const auto ticks  = roundToInt (std::fmod (beats, 1.0) * 960.0 + 0.5);

    return String::formatted ("%d|%d|%03d", bar, beat, ticks);
}

/** @returns a textual description of a CurrentPositionInfo. */
[[nodiscard]] inline String getTimecodeDisplay (const AudioPlayHead::CurrentPositionInfo& pos)
{
    String displayText;

    displayText << String (pos.bpm, 2) << " bpm, "
                << pos.timeSigNumerator << '/' << pos.timeSigDenominator
                << "  -  " << timeToTimecodeString (pos.timeInSeconds)
                << "  -  " << quarterNotePositionToBarsBeatsString (pos.ppqPosition,
                                                                    pos.timeSigNumerator,
                                                                    pos.timeSigDenominator);

    if (pos.isRecording)
        displayText << "  (recording)";
    else if (pos.isPlaying)
        displayText << "  (playing)";
    else
        displayText << "  (stopped)";

    return displayText;
}

//==============================================================================
/** */
struct ParameterGesturer final
{
    /** */
    ParameterGesturer (AudioProcessorParameter& p) :
        parameter (p)
    {
        parameter.beginChangeGesture();
    }

    /** */
    ~ParameterGesturer()
    {
        parameter.endChangeGesture();
    }

    AudioProcessorParameter& parameter;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterGesturer)
};
