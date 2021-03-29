/**
    @file MillisecondStopWatch.h
    @copyright Noteloop Systems Inc.

    A simple class that can be used for precise timing, in milliseconds.

    @see StopWatch, TickStopWatch
*/
class MillisecondStopWatch : public StopWatch<double>
{
public:
    /** Constructor */
    MillisecondStopWatch() noexcept
    {
    }

    //==============================================================================
    /** @internal */
    double getCurrentTime() const override
    {
        return Time::getMillisecondCounterHiRes();
    }

private:
    //==============================================================================
    JUCE_LEAK_DETECTOR (MillisecondStopWatch)
};
