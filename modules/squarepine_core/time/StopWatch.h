/** A base-class for designing stop-watches.

    @see MillisecondStopWatch, TickStopWatch
*/
template<typename Type>
class StopWatch
{
public:
    /** Constructor */
    StopWatch() = default;

    /** Destructor */
    virtual ~StopWatch() = default;

    //==============================================================================
    /** Starts the stop-watch. */
    void start (bool forceReset = false)
    {
        if (! running || forceReset)
        {
            startTime = getCurrentTime();
            running = true;
        }
    }

    /** Stops the stop-watch */
    void stop()
    {
        jassert (running);

        if (running)
        {
            stopTime = getCurrentTime();
            running = false;
        }
    }

    //==============================================================================
    /** @returns the time for when the stopwatch started */
    Type getStartTime() const noexcept { return startTime; }

    /** @returns the time for when the stopwatch stopped */
    Type getStopTime() const noexcept { return stopTime; }

    /** @returns the difference between the start and stop time. */
    Type getDelta() const noexcept { return stopTime - startTime; }

protected:
    //==============================================================================
    /** Implement this in your subclass */
    virtual Type getCurrentTime() const = 0;

private:
    //==============================================================================
    Type startTime = {}, stopTime = {};
    bool running = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StopWatch)
};

//==============================================================================
/** A handy RAII-based stop-watch starter/stopper. */
template<class StopWatchType>
class ScopedStartStop final
{
public:
    /** Constructor, which will automatically start the stop-watch */
    ScopedStartStop (StopWatchType& owner) :
        sw (owner)
    {
        sw.start (true);
    }

    /** Destructor, which will automatically stop the stop-watch. */
    ~ScopedStartStop()
    {
        sw.stop();
    }

private:
    //==============================================================================
    StopWatchType& sw;

    //==============================================================================
    ScopedStartStop() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedStartStop)
};

//==============================================================================
/** A simple class that can be used for precise timing, in milliseconds.

    @see StopWatch, TickStopWatch
*/
class MillisecondStopWatch final : public StopWatch<double>
{
public:
    /** Constructor. */
    MillisecondStopWatch() noexcept = default;

    //==============================================================================
    /** @internal */
    double getCurrentTime() const override { return Time::getMillisecondCounterHiRes(); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MillisecondStopWatch)
};

//==============================================================================
/** A simple class that can be used for precise timing, in CPU ticks.

    @see StopWatch, TickStopWatch
*/
class TickStopWatch final : public StopWatch<int64>
{
public:
    /** Constructor */
    TickStopWatch() noexcept = default;

    //==============================================================================
    /** @internal */
    int64 getCurrentTime() const override { return Time::getHighResolutionTicks(); }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TickStopWatch)
};
