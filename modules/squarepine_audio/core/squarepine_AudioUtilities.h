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
/** @returns true if a buffer is actually cleared; not just by its clear flag. */
template<typename FloatType>
[[nodiscard]] inline bool isCleared (const juce::AudioBuffer<FloatType>& buffer)
{
    if (buffer.hasBeenCleared())
        return true;

    const int numSamples = buffer.getNumSamples();
    if (auto channels = buffer.getArrayOfReadPointers())
        for (int i = buffer.getNumChannels(); --i >= 0;)
            if (auto channel = channels[i])
                for (int f = numSamples; --f >= 0;)
                    if (! approximatelyEqual (channel[f], FloatType()))
                        return false;

    return true;
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
inline void addFrom (juce::AudioBuffer<FloatType>& destination,
                     juce::AudioBuffer<FloatType>& source,
                     int numChannels, int numSamples,
                     FloatType gainFactor = (FloatType) 1)
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
inline void addFrom (juce::AudioBuffer<FloatType>& destination,
                     juce::AudioBuffer<FloatType>& source,
                     FloatType gainFactor = (FloatType) 1)
{
    addFrom (destination, source,
             std::min (source.getNumChannels(), destination.getNumChannels()),
             std::min (source.getNumSamples(), destination.getNumSamples()),
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
class ScopedParameterGesturer final
{
public:
    /** */
    ScopedParameterGesturer (AudioProcessorParameter& p) :
        parameter (p)
    {
        parameter.beginChangeGesture();
    }

    /** */
    ~ScopedParameterGesturer()
    {
        parameter.endChangeGesture();
    }

private:
    AudioProcessorParameter& parameter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedParameterGesturer)
};

//==============================================================================
/** @returns an array of pointers to a processor's parameters,
    excluding its bypass parameter if set.
*/
inline Array<AudioProcessorParameter*> getAllParametersExcludingBypass (AudioProcessor& processor)
{
    auto params = processor.getParameters();
    params.removeAllInstancesOf (processor.getBypassParameter());
    return params;
}

//==============================================================================
/** An RAII mechanism that automatically suspends/unsuspends an AudioProcessor. */
class ScopedSuspend final
{
public:
    /** Begins suspending the audio processor if it wasn't already. */
    ScopedSuspend (AudioProcessor& audioProcessor) :
        proc (audioProcessor),
        wasSuspended (proc.isSuspended())
    {
        if (! wasSuspended)
            proc.suspendProcessing (true);
    }

    /** Restores the last suspension state. */
    ~ScopedSuspend()
    {
        if (! wasSuspended)
            proc.suspendProcessing (false);
    }

private:
    AudioProcessor& proc;
    const bool wasSuspended;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedSuspend)
};

//==============================================================================
/** @returns the RMS for a set of raw values.
    @warning This does very little validation so be careful!
*/
template<typename Type, typename SizeType = int>
[[nodiscard]] inline Type findRMS (Type* rawValues, SizeType numValues)
{
    constexpr auto zero = static_cast<Type> (0);
    if (rawValues == nullptr || numValues <= 0)
        return zero;

    auto sum = zero;
    for (SizeType i = 0; i < numValues; ++i)
        sum += square (rawValues[i]);

    return std::sqrt (sum / static_cast<Type> (numValues));
}

/** @returns the RMS for a set of values. */
template<typename Iterator>
[[nodiscard]] inline typename std::iterator_traits<Iterator>::value_type findRMS (Iterator beginIter, Iterator endIter)
{
    auto sum = static_cast<typename std::iterator_traits<Iterator>::value_type> (0);

    for (auto iter = beginIter; iter != endIter; ++iter)
        sum += square (*iter);

    return std::sqrt (sum / std::distance (beginIter, endIter));
}

/** @returns the RMS for a set of values. */
template<typename Type, typename TypeOfCriticalSectionToUse>
[[nodiscard]] inline Type findRMS (const juce::Array<Type, TypeOfCriticalSectionToUse>& values)
{
    return findRMS (std::cbegin (values), std::cend (values));
}

/** @returns the RMS for a set of values. */
template<typename Type, typename TypeOfCriticalSectionToUse>
[[nodiscard]] inline Type findRMS (const juce::OwnedArray<Type, TypeOfCriticalSectionToUse>& values)
{
    return findRMS (std::cbegin (values), std::cend (values));
}

/** @returns the RMS for a set of values. */
template<typename Type, typename AllocatorType>
[[nodiscard]] inline Type findRMS (const std::vector<Type, AllocatorType>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, typename AllocatorType>
[[nodiscard]] inline Type findRMS (const std::list<Type, AllocatorType>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, typename AllocatorType>
inline Type findRMS (const std::forward_list<Type, AllocatorType>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, size_t arraySize>
[[nodiscard]] inline Type findRMS (const std::array<Type, arraySize>& values)
{
    return findRMS (values.cbegin(), values.cend());
}

/** @returns the RMS for a set of values. */
template<typename Type, size_t arraySize>
[[nodiscard]] inline Type findRMS (const Type (&values)[arraySize])
{
    return findRMS (std::cbegin (values), std::cend (values));
}

//============================================================================
/** @returns a pitch ratio converted from a number of semitones. */
[[nodiscard]] inline float semitonesToPitchRatio (float numSemitones) noexcept    { return std::pow (2.0f, numSemitones / 12.0f); }
/** @returns a pitch ratio converted from a number of semitones. */
[[nodiscard]] inline double semitonesToPitchRatio (double numSemitones) noexcept  { return std::pow (2.0, numSemitones / 12.0); }
/** @returns a pitch ratio converted from a number of semitones. */
template<typename Type>
[[nodiscard]] inline double semitonesToPitchRatio (Type numSemitones) noexcept    { return semitonesToPitchRatio (static_cast<double> (numSemitones)); }

/** @returns a number semitones converted from a pitch ratio. */
[[nodiscard]] inline float pitchRatioToSemitones (float pitchRatio) noexcept      { return 12.0f * std::log2 (pitchRatio); }
/** @returns a number semitones converted from a pitch ratio. */
[[nodiscard]] inline double pitchRatioToSemitones (double pitchRatio) noexcept    { return 12.0 * std::log2 (pitchRatio); }
/** @returns a number semitones converted from a pitch ratio. */
template<typename Type>
[[nodiscard]] inline double pitchRatioToSemitones (Type pitchRatio) noexcept      { return pitchRatioToSemitones (static_cast<double> (pitchRatio)); }

//============================================================================
/** @returns a MIDI note converted from a frequency (in Hz). */
[[nodiscard]] inline int frequencyToMIDINote (double frequencyHz) noexcept
{
    const auto pitchRatio = std::max (0.0, frequencyHz / 440.0);
    return roundToInt (69.0 + pitchRatioToSemitones (pitchRatio));
}

/** @returns a frequency (in Hz) converted from a MIDI note. */
[[nodiscard]] inline double midiNoteToFrequency (int midiNoteNumber) noexcept
{
    const auto semitones = std::max (0, midiNoteNumber);
    return 440.0 * semitonesToPitchRatio (static_cast<double> (semitones) - 69.0);
}
