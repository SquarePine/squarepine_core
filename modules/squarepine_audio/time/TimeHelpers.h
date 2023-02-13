/** */
constexpr double determineTempo (double secondsPerQuarterNote) noexcept
{
    return secondsPerQuarterNote > 0.0
            ? (60.0 / secondsPerQuarterNote)
            : 0.0;
}

/** */
constexpr double determineSecondsPerQuarterNote (double tempo) noexcept
{
    return tempo > 0.0
            ? (60.0 / tempo)
            : 0.0;
}

/** */
constexpr double determineSecondsPerMeasure (double quarterNotesPerMeasure, double tempo) noexcept
{
    return (quarterNotesPerMeasure > 0.0 && tempo > 0.0)
            ? (quarterNotesPerMeasure * determineSecondsPerQuarterNote (tempo))
            : 0.0;
}

/** */
constexpr double convertSecondsToMeasures (double seconds, double tempo, double quarterNotesPerMeasure) noexcept
{
    return seconds / determineSecondsPerMeasure (quarterNotesPerMeasure, tempo);
}

/** */
constexpr double convertQuarterNotesToSeconds (double quarterNotes, double tempo) noexcept
{
    return (quarterNotes >= 0.0 && tempo >= 0.0)
            ? quarterNotes * determineSecondsPerQuarterNote (tempo)
            : 0.0;
}
/** */
constexpr double convertSecondsToQuarterNotes (double seconds, double tempo) noexcept
{
    // TODO: this needs to handle tempo automation
    return (tempo >= 0.0)
            ? (seconds / determineSecondsPerQuarterNote (tempo))
            : 0.0;
}

/** */
constexpr double convertTimestampToPixels (double timestampSeconds, double tempo, double quarterNoteWidth) noexcept
{
    return (tempo >= 0.0 && quarterNoteWidth > 0.0)
            ? (convertSecondsToQuarterNotes (timestampSeconds, tempo) * quarterNoteWidth)
            : 0.0;
}

/** */
constexpr double convertPixelsToTimestamp (int timestampPixels, double tempo, double quarterNoteWidth) noexcept
{
    return (timestampPixels >= 0 && tempo >= 0.0 && quarterNoteWidth > 0.0)
            ? (((double) timestampPixels / quarterNoteWidth) * determineSecondsPerQuarterNote (tempo))
            : 0.0;
}

/** */
inline int convertSnapIncrementToPixels (int snapPositionX, double snapIncrement) noexcept
{
    return roundToIntAccurate (roundToIntAccurate ((double) snapPositionX / snapIncrement) * snapIncrement);
}

/** */
inline double determineSnapPointAsTimestamp (int snapPosition, double tempo, double quarterNoteWidth, double quarterNotesPerSnap) noexcept
{
    const auto noteWidth = quarterNoteWidth * quarterNotesPerSnap;
    const auto secondsPerNote = determineSecondsPerQuarterNote (tempo) * quarterNotesPerSnap;

    return roundToIntAccurate ((double) snapPosition / noteWidth) * secondsPerNote;
}

/** */
constexpr double determineBarLengthInTicks (int numerator, int denominator, int ppq = 960) noexcept
{
    return (((double) numerator / (double) denominator) * 4.0) * (double) ppq;
}

/** */
constexpr double secondsToTicks (double timeStamp, double tempo, int ppq = 960) noexcept
{
    return ((double) ppq / (60.0 / tempo)) * timeStamp;
}

/** */
constexpr double ticksToSeconds (double timeStamp, double tempo, int ppq = 960) noexcept
{
    return (60.0 / (tempo * (double) ppq)) * timeStamp;
}

/** */
inline double ticksToSeconds (double time, const MidiMessageSequence& tempoAndTSEvents, int timeFormat)
{
    if (timeFormat > 0)
    {
        const auto tickLen  = 1.0 / (timeFormat & 0x7fff);
        const auto numEvents = tempoAndTSEvents.getNumEvents();
        auto lastTime       = 0.0;
        auto correctedTime  = 0.0;
        auto secsPerTick    = 0.5 * tickLen;

        for (int i = 0; i < numEvents; ++i)
        {
            const auto& m = tempoAndTSEvents.getEventPointer (i)->message;
            const auto eventTime = m.getTimeStamp();

            if (eventTime >= time)
                break;

            correctedTime += (eventTime - lastTime) * secsPerTick;
            lastTime = eventTime;

            if (m.isTempoMetaEvent())
                secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();

            while (i + 1 < numEvents)
            {
                const auto& m2 = tempoAndTSEvents.getEventPointer (i + 1)->message;

                if (m2.getTimeStamp() != eventTime)
                    break;

                if (m2.isTempoMetaEvent())
                    secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();

                ++i;
            }
        }

        return correctedTime + (time - lastTime) * secsPerTick;
    }

    return time / (((timeFormat & 0x7fff) >> 8) * (timeFormat & 0xff));
}

/** */
template<typename Type = int64>
constexpr double timeSamplesToSeconds (Type samplePos, double sampleRate) noexcept
{
    return sampleRate <= 0.0
            ? 0.0
            : static_cast<double> (samplePos) / sampleRate;
}

/** */
constexpr double timeSamplesToSeconds (const AudioFormatReader& reader) noexcept
{
    return timeSamplesToSeconds (reader.lengthInSamples, reader.sampleRate);
}

/** */
template<typename Type = int64>
constexpr Type timeSecondsToSamples (double timeSeconds, double sampleRate) noexcept
{
    return sampleRate <= 0.0
            ? static_cast<Type> (0)
            : static_cast<Type> (timeSeconds * sampleRate);
}

/** */
constexpr double millisecondsToPpq (double timeMs, double tempo) noexcept
{
    return tempo / 60.0 * (timeMs / 1000.0);
}

/** */
constexpr double ppqToSeconds (double ppq, double tempo) noexcept
{
    return ppq / (tempo / 60.0);
}
