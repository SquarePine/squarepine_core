//==============================================================================
/** Use an instance of this to track a time-signature.

    @see Tempo, Beats
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

    //==============================================================================
    /** The default denominator in beats. */
    static constexpr int defaultDenominator = 4;

    /** The minimum denominator. */
    static constexpr int mininumDenominator = 1;

    /** The maximum denominator.
        This value is a power of two, specifically 2 ^ 16.
    */
    static constexpr int maximumDenominator = 65536;

    //==============================================================================
    /** Creates a default time signature of 4:4. */
    TimeSignature() noexcept = default;

    /** Creates a time signature using provided values.

        @warning This will snap the values into range if you're up to something funky!

        @see defaultNumerator, defaultDenominator
    */
    TimeSignature (int numerator, int denominator) noexcept;

    /** Creates a copy of another time signature. */
    TimeSignature (const TimeSignature&) noexcept = default;

    /** Creates a copy of another time signature. */
    TimeSignature (TimeSignature&&) noexcept = default;

    /** Destructor. */
    ~TimeSignature() noexcept = default;

    //==============================================================================
    /** Creates a copy of this time signature swapping out
        its numerator for the provided one.

        @warning The provided numerator will be snapped if it's out of range!
    */
    TimeSignature withNumerator (int n) const;

    /** Creates a copy of this time signature swapping out
        its denominator for the provided one.

        @warning The provided denominator will be snapped if it's out of range!
    */
    TimeSignature withDenominator (int d) const;

    //==============================================================================
    /** */
    double getNumQuarterNotesPerMeasure() const noexcept;

    /** */
    double getNumSecondsPerMeasure (const Tempo&) const noexcept;

    /** */
    double getNumSecondsPerMeasure (double tempo) const noexcept;

    //==============================================================================
    /** Tries to find a global time signature from the provided MidiFile. */
    static TimeSignature getTimeSignature (const MidiFile&);

    /** Tries to find a global time signature from the provided AudioFormatReader. */
    static TimeSignature getTimeSignature (const AudioFormatReader&);

    //==============================================================================
    /** */
    String toString() const;

    /** */
    static TimeSignature fromString (const String&);

    //==============================================================================
    /** */
    TimeSignature& operator= (const TimeSignature&) noexcept = default;
    /** */
    TimeSignature& operator= (TimeSignature&&) noexcept = default;
    /** */
    bool operator== (const TimeSignature&) const noexcept;
    /** */
    bool operator!= (const TimeSignature&) const noexcept;
    /** */
    bool operator< (const TimeSignature&) const noexcept;
    /** */
    bool operator<= (const TimeSignature&) const noexcept;
    /** */
    bool operator> (const TimeSignature&) const noexcept;
    /** */
    bool operator>= (const TimeSignature&) const noexcept;

    //==============================================================================
    int numerator = defaultNumerator,       // The '7' of '7:8'.
        denominator = defaultDenominator;   // The '8' of '7:8'.

private:
    //==============================================================================
    void snapToRange() noexcept;
};

//==============================================================================
/** Use the methods provided to help calculate beat related information,
    as well as mapping to various units of time like seconds, and even pixels.

    @see Tempo, TimeSignature
*/
class Beats final
{
public:
    //==============================================================================
    /** The suggested lowest interval in beats, which is in 8192th notes. */
    static constexpr auto resolution = 1.0 / 8192.0;

    //==============================================================================
    /** A convenient value describing a large note. */
    static constexpr auto largeNote = 8.0;

    /** A convenient value describing a long note. */
    static constexpr auto longNote = 4.0;

    /** A convenient value describing a double whole note. */
    static constexpr auto doubleWholeNote = 2.0;

    /** A convenient value describing a whole note. */
    static constexpr auto wholeNote = 1.0;

    /** A convenient value describing a half note. */
    static constexpr auto halfNote = wholeNote / 2.0;

    /** A convenient value describing a quarter note. */
    static constexpr auto quarterNote = wholeNote / 4.0;

    /** A convenient value describing an 8th note. */
    static constexpr auto eighthNote = wholeNote / 8.0;

    /** A convenient value describing a 16th note. */
    static constexpr auto sixteenthNote = wholeNote / 16.0;

    /** A convenient value describing a 32nd note. */
    static constexpr auto thirtySecondNote = wholeNote / 32.0;

    /** A convenient value describing a 64th note. */
    static constexpr auto sixtyFourthNote = wholeNote / 64.0;

    /** A convenient value describing a 128th note. */
    static constexpr auto oneHundredTwentyEighthNote = wholeNote / 128.0;

    /** A convenient value describing a 256th note. */
    static constexpr auto twoHundredFiftySixthNote = wholeNote / 256.0;

    //==============================================================================
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

    /** @returns a beat representing a position in pixels. */
    static double fromPixels (double pixels, double ppb = pixelsPerBeat) noexcept
    {
        return static_cast<double> (pixels) / toPixelsAccurate (1.0, ppb);
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
        return secondsToSamples<int64> (toSeconds (tempo, timeSignature), sampleRate);
    }

    /** @returns what time a beat will be in audio samples. */
    static constexpr int64 toSamples (double beats, const Tempo& tempo, const TimeSignature& timeSignature, double sampleRate) noexcept
    {
        return secondsToSamples<int64> (toSeconds (beats, tempo, timeSignature), sampleRate);
    }

    /** @returns a "beat" corresponding to the provided samples. */
    static constexpr double fromSamples (int64 samples, const Tempo& tempo, double sampleRate) noexcept
    {
        return fromSeconds (samplesToSeconds (samples, sampleRate), tempo);
    }

private:
    //==============================================================================
    SQUAREPINE_DECLARE_TOOL_CLASS (Beats)
};
