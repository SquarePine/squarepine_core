/** Object capable of denoting time in various formats */
class TimeKeeper final
{
public:
    /** */
    TimeKeeper (double sampleRate = 44100.0);

    /** */
    TimeKeeper (const AudioPlayHead::PositionInfo&,
                double sampleRate = 44100.0);
    //==============================================================================
    /** */
    TimeKeeper& setSamplingRate (double sampleRate);
    /** */
    TimeKeeper& setSamplingRate (AudioDeviceManager&);

    //==============================================================================
    /** */
    TimeKeeper& setTime (double timeSeconds);
    /** */
    TimeKeeper& setTime (int64 timeInSamples);
    /** */
    TimeKeeper& setTime (int minutes, int seconds, int milliseconds);
    /** */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int milliseconds);
    /** */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int frames,
                         MidiMessage::SmpteTimecodeType);
    /** */
    TimeKeeper& setTime (int hours, int minutes, int seconds, int frames,
                         AudioPlayHead::FrameRateType);

    /** */
    TimeKeeper& setFrameRate (MidiMessage::SmpteTimecodeType);
    /** */
    TimeKeeper& setFrameRate (AudioPlayHead::FrameRateType);

    /** */
    TimeKeeper& setTempo (const Tempo&);
    /** */
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

    /** */
    TimeKeeper& setTimeFormat (TimeFormat);
    /** */
    TimeFormat getCurrentTimeFormat() const noexcept { return timeFormat; }

    //==============================================================================
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
