/** */
class DecimalTime final : public TimeFormat
{
public:
    /** */
    DecimalTime (double timeInSeconds) noexcept;
    /** */
    DecimalTime (int64 timeInSamples, double sampleRate) noexcept;
    /** */
    DecimalTime (int minutes, int seconds, int milliseconds) noexcept;
    /** */
    DecimalTime (int hours, int minutes, int seconds, int milliseconds) noexcept;

    //==============================================================================
    /** */
    static int hoursToSeconds (int hours) noexcept;
    /** */
    static int minutesToSeconds (int minutes) noexcept;

    //==============================================================================
    /** */
    DecimalTime& operator= (const DecimalTime& other) noexcept;
    /** */
    bool operator== (const DecimalTime& other) const noexcept;
    /** */
    bool operator!= (const DecimalTime& other) const noexcept;

    //==============================================================================
    /** @internal */
    double toSeconds() const override;
    /** @internal */
    String toString() const override;

private:
    //==============================================================================
    double timeInSeconds;

    //==============================================================================
    JUCE_LEAK_DETECTOR (DecimalTime)
};
