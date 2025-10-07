/** Object capable of denoting time in various formats. */
class TimeKeeper final
{
public:
    /** */
    TimeKeeper (double sampleRate = 44100.0);

    /** */
    TimeKeeper (const AudioPlayHead::CurrentPositionInfo&,
                double sampleRate = 44100.0);

    //==============================================================================
    /** @returns */
    TimeKeeper& setSamplingRate (double sampleRate);
    /** @returns */
    TimeKeeper& setSamplingRate (AudioDeviceManager&);

    //==============================================================================
    /** @returns */
    TimeKeeper& setTime (double timeSeconds);
    /** @returns */
    TimeKeeper& setTime (int64 timeInSamples);
    /** @returns */
    TimeKeeper& setTime (int minutes, int seconds, int milliseconds);
    /** @returns */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int milliseconds);
    /** @returns */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int frames,
                         MidiMessage::SmpteTimecodeType);
    /** @returns */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int frames,
                         AudioPlayHead::FrameRateType);

    /** @returns */
    TimeKeeper& setFrameRate (MidiMessage::SmpteTimecodeType);
    /** @returns */
    TimeKeeper& setFrameRate (AudioPlayHead::FrameRateType);

    /** @returns */
    TimeKeeper& setTempo (const Tempo&);
    /** @returns */
    TimeKeeper& setTimeSignature (const TimeSignature&);

    //==============================================================================
    /** */
    enum class TimeFormat
    {
        smpteTime,
        decimalTime,
        secondsTime,
        samplesTime,
        measuresBeatsTicks
    };

    /** @returns */
    TimeKeeper& setTimeFormat (TimeFormat);
    /** @returns */
    [[nodiscard]] TimeFormat getTimeFormat() const noexcept { return timeFormat; }

    //==============================================================================
    /** @returns */
    String toString() const;

private:
    //==============================================================================
    TimeFormat timeFormat = TimeFormat::decimalTime;
    double frameRate = 60.0, sampleRate = 44100.0, timeSeconds = 0.0;
    Tempo tempo;
    TimeSignature timeSignature;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeKeeper)
};
