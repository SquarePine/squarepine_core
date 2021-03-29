/** A base-class for designing stop-watches

    @see MillisecondStopWatch, TickStopWatch
*/
template<typename Type>
class StopWatch
{
public:
    /** Constructor */
    StopWatch() :
        startTime (Type()),
        stopTime (Type()),
        running (false)
    {
    }

    /** Destructor */
    virtual ~StopWatch()
    {
    }

    //==============================================================================
    /** Starts the stop-watch */
    void start()
    {
        jassert (! running);

        if (! running)
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
    Type startTime, stopTime;
    bool running;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StopWatch)
};

//==============================================================================
/** A handy RAII-based stop-watch starter/stopper */
template<class StopWatchType>
class ScopedStartStop
{
public:
    /** Constructor, which will start the stop-watch */
    inline ScopedStartStop (StopWatchType& owner) :
        sw (owner)
    {
        sw.start();
    }

    /** Destructor, which will stop the stop-watch */
    inline ~ScopedStartStop()
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
