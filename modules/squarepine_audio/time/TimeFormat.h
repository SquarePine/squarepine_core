//==============================================================================
/** */
static constexpr int secondsPerHour = 3600;

/** */
static constexpr int secondsPerMinute = 60;

//==============================================================================
/** */
class TimeFormat
{
public:
    /** */
    TimeFormat() noexcept = default;

    /** */
    virtual ~TimeFormat() noexcept = default;

    //==============================================================================
    /** */
    virtual double toSeconds() const = 0;

    /** */
    virtual String toString() const = 0;

private:
    JUCE_LEAK_DETECTOR (TimeFormat)
};
