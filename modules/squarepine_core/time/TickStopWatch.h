/** A simple class that can be used for precise timing, in CPU ticks.

    @see StopWatch, TickStopWatch
*/
class TickStopWatch : public StopWatch<int64>
{
public:
    /** Constructor */
    TickStopWatch() noexcept
    {
    }

    //==============================================================================
    /** @internal */
    int64 getCurrentTime() const override
    {
        return Time::getHighResolutionTicks();
    }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (TickStopWatch)
};
