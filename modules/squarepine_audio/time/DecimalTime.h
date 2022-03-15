/** */
class DecimalTime final : public TimeFormat
{
public:
    /** */
    DecimalTime() noexcept = default;
    /** */
    DecimalTime (double timeInSeconds) noexcept;
    /** */
    DecimalTime (int64 timeInSamples, double sampleRate) noexcept;
    /** */
    DecimalTime (int minutes, int seconds, int milliseconds) noexcept;
    /** */
    DecimalTime (int hours, int minutes, int seconds, int milliseconds) noexcept;
    /** */
    DecimalTime (const DecimalTime&) noexcept = default;
    /** */
    ~DecimalTime() noexcept override = default;

    //==============================================================================
    /** */
    static constexpr int hoursToSeconds (int hours) noexcept;
    /** */
    static constexpr int minutesToSeconds (int minutes) noexcept;

    //==============================================================================
    /** */
    DecimalTime& operator= (const DecimalTime&) noexcept;
    /** */
    bool operator== (const DecimalTime&) const noexcept;
    /** */
    bool operator!= (const DecimalTime&) const noexcept;

    //==============================================================================
    /** @internal */
    double toSeconds() const override { return timeSeconds; }
    /** @internal */
    String toString() const override;

private:
    //==============================================================================
    double timeSeconds = 0.0;

    //==============================================================================
    JUCE_LEAK_DETECTOR (DecimalTime)
};
