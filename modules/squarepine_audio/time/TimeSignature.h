//==============================================================================
/** Use an instance of this to track a time-signature.

    @see Tempo, Beat
*/
class TimeSignature final
{
public:
    //==============================================================================
    /** The default numerator in beats. */
    static constexpr int defaultNumerator = 4;

    /** The minimum numerator. */
    static constexpr int mininumNumerator = 1;

    /** The maximum numerator. */
    static constexpr int maximumNumerator = 256;

    /** The default denominator in beats. */
    static constexpr int defaultDenominator = 4;

    /** The minimum denominator. */
    static constexpr int mininumDenominator = 1;

    /** The maximum denominator.
        This value is a power of two, specifically 2 ^ 16.
    */
    static constexpr int maximumDenominator = 65536;

    //==============================================================================
    /** */
    TimeSignature() noexcept = default;

    /** */
    TimeSignature (int numerator, int denominator) noexcept;

    /** */
    TimeSignature (const TimeSignature&) noexcept = default;

    /** */
    TimeSignature (TimeSignature&&) noexcept = default;

    /** Destructor. */
    ~TimeSignature() noexcept = default;

    //==============================================================================
    /** */
    TimeSignature withNumerator (int n) const;

    /** */
    TimeSignature withDenominator (int d) const;

    //==============================================================================
    /** */
    double getNumQuarterNotesPerMeasure() const noexcept;

    /** */
    double getNumSecondsPerMeasure (const Tempo& tempo) const noexcept;

    /** */
    double getNumSecondsPerMeasure (double tempo) const noexcept;

    //==============================================================================
    /** */
    static TimeSignature getTimeSignature (const MidiFile& midiFile)
    {
        for (int i = 0; i < midiFile.getNumTracks(); ++i)
        {
            auto track = *midiFile.getTrack (i);
            track.sort();

            for (auto* meh : track)
            {
                const auto& msg = meh->message;
                if (msg.isTimeSignatureMetaEvent())
                {
                    int n = 0, d = 0;
                    msg.getTimeSignatureInfo (n, d);
                    return { n, d };
                }
            }
        }

        return {};
    }

    /** */
    static TimeSignature getTimeSignature (const AudioFormatReader& reader)
    {
        TimeSignature timeSig;

        const auto& metadata = reader.metadataValues;

        if (reader.getFormatName().containsIgnoreCase ("WAV"))
        {
            if (metadata.containsKey (WavAudioFormat::acidNumerator))   timeSig.numerator = metadata[WavAudioFormat::acidNumerator].getIntValue();
            if (metadata.containsKey (WavAudioFormat::acidDenominator)) timeSig.denominator = metadata[WavAudioFormat::acidDenominator].getIntValue();
        }
       #if SQUAREPINE_USE_REX_AUDIO_FORMAT
        else if (reader.getFormatName().containsIgnoreCase ("REX"))
        {
            if (metadata.containsKey (REXAudioFormat::rexNumerator))    timeSig.numerator = metadata[REXAudioFormat::rexNumerator].getIntValue();
            if (metadata.containsKey (REXAudioFormat::rexDenominator))  timeSig.denominator = metadata[REXAudioFormat::rexDenominator].getIntValue();
        }
       #endif
       #if JUCE_MAC
        else if (reader.getFormatName().containsIgnoreCase ("CoreAudio"))
        {
            if (metadata.containsKey (CoreAudioFormat::timeSig))        timeSig = TimeSignature::fromString (metadata[CoreAudioFormat::timeSig]);
        }
       #endif

        return timeSig;
    }

    //==============================================================================
    /** */
    String toString() const;

    /** */
    static TimeSignature fromString (const String& possibleString);

    //==============================================================================
    /** */
    TimeSignature& operator= (const TimeSignature&) noexcept = default;
    /** */
    TimeSignature& operator= (TimeSignature&&) noexcept = default;
    /** */
    bool operator== (const TimeSignature& other) const noexcept;
    /** */
    bool operator!= (const TimeSignature& other) const noexcept;
    /** */
    bool operator< (const TimeSignature& other) const noexcept;
    /** */
    bool operator<= (const TimeSignature& other) const noexcept;
    /** */
    bool operator> (const TimeSignature& other) const noexcept;
    /** */
    bool operator>= (const TimeSignature& other) const noexcept;

    //==============================================================================
    int numerator = defaultNumerator;       //< The '7' of '7:8'.
    int denominator = defaultDenominator;   //< The '8' of '7:8'.

private:
    //==============================================================================
    void snapToRange();
};

//==============================================================================
/** Use the methods provided to help calculate beat related information.

    @see Tempo, TimeSignature
*/
class Beats final
{
public:
    //==============================================================================
    /** The tempo interval in beats, which is in 8192th notes. */
    static constexpr double resolution = 1.0 / 8192.0;

    /** A default number of pixels per beat which can be used to place items on a grid. */
    static constexpr auto pixelsPerBeat = 64.0;

    //==============================================================================
    /** @returns what time a beat will be in pixels (and subpixels). */
    static constexpr double toPixelsAccurate (double beats, double ppb = pixelsPerBeat) noexcept
    {
        return beats * ppb;
    }

    /** @returns what time a beat will be in pixels, rounded or snapped to the nearest pixel. */
    static int toPixels (double beats, double ppb = pixelsPerBeat) noexcept
    {
        return (int) std::round (toPixelsAccurate (beats, ppb));
    }

    //==============================================================================
    /** @returns how long a beat will be in milliseconds. */
    static constexpr double toMilliseconds (const Tempo& tempo, const TimeSignature& timeSignature) noexcept
    {
        return 60000.0 / tempo.get() / timeSignature.denominator;
    }

    /** @returns what time a beat will be in milliseconds. */
    static constexpr double toMilliseconds (double beats, const Tempo& tempo, const TimeSignature& timeSignature) noexcept
    {
        return beats * toMilliseconds (tempo, timeSignature);
    }

    /** @returns a "beat" corresponding to the provided milliseconds. */
    static constexpr double fromMilliseconds (double milliseconds, const Tempo& tempo) noexcept
    {
        return fromSeconds (milliseconds / 1000.0, tempo);
    }

    //==============================================================================
    /** @returns how long a beat will be in seconds. */
    static constexpr double toSeconds (const Tempo& tempo, const TimeSignature& timeSignature) noexcept
    {
        return toMilliseconds (tempo, timeSignature) / 1000.0;
    }

    /** @returns what time a will would be in seconds. */
    static constexpr double toSeconds (double beats, const Tempo& tempo, const TimeSignature& timeSignature) noexcept
    {
        return beats * toSeconds (tempo, timeSignature);
    }

    /** @returns a "beat" corresponding to the provided seconds. */
    static constexpr double fromSeconds (double seconds, const Tempo& tempo) noexcept
    {
        return seconds / (60.0 / tempo.get());
    }

    //==============================================================================
    /** @returns how long a beat will be in audio samples. */
    static constexpr int64 toSamples (const Tempo& tempo, const TimeSignature& timeSignature, double sampleRate) noexcept
    {
        return timeSecondsToSamples<int64> (toSeconds (tempo, timeSignature), sampleRate);
    }

    /** @returns what time a beat will be in audio samples. */
    static constexpr int64 toSamples (double beats, const Tempo& tempo, const TimeSignature& timeSignature, double sampleRate) noexcept
    {
        return timeSecondsToSamples<int64> (toSeconds (beats, tempo, timeSignature), sampleRate);
    }

    /** @returns a "beat" corresponding to the provided samples. */
    static constexpr double fromSamples (int64 samples, const Tempo& tempo, double sampleRate) noexcept
    {
        return fromSeconds (timeSamplesToSeconds (samples, sampleRate), tempo);
    }

private:
    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (Beats)
};
