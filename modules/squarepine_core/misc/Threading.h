/** Attempts shutting down a thread safely (ie: without killing it mercilessly).

    @param thread   The thread we're trying to shutdown cleanly and safely.
    @param timeout  The time to wait, in milliseconds, for the thread to try
                    to shutdown. If this value is less than zero, it will wait forever.
*/
inline void shutdownThreadSafely (Thread& thread, std::chrono::milliseconds timeout = std::chrono::milliseconds (3000))
{
    if (! thread.isThreadRunning())
          return; // Nothing to do...

      thread.signalThreadShouldExit();
      thread.notify();

      while (! thread.waitForThreadToExit (static_cast<int> (timeout.count())))
      {
          thread.signalThreadShouldExit();
          thread.notify();
      }
}

//==============================================================================
/** Makes the calling thread wait until a specified time.

    This will cause the calling thread to "idle-wait" until the
    given target time is reached. The goal of this function is to be as
    light on the processor as possible, while trying to return as close to
    the given target time as possible.

    This function will sleep() for 1 ms if the time to target
    is greater than the given sleep threshold.

    If there is not enough time to call sleep, the thread will simply yield.

    @param targetTime The time in ticks after which the function should return
    @param sleepThresholdMS The minimum amount of time that must be remaining in order for a sleep call to happen.
                                If 0, no sleep calls will be made.
*/
inline void waitUntilTime (int64 targetTime, uint32 sleepThresholdMS = 0)
{
    const int64 threshold = sleepThresholdMS > 0
        ? Time::getHighResolutionTicksPerSecond() / (1000 / sleepThresholdMS)
        : 0;

    int64 timeLeft = 0; //The time remaining before we should return

    //If we haven't reached the target time yet, then wait.
    while ((timeLeft = targetTime - Time::getHighResolutionTicks()) > 0)
    {
        //If there is more than the threshold remaining, we should sleep to reduce CPU usage:
        if (threshold != 0 && timeLeft > threshold)
            Thread::sleep (1);
        else
            Thread::yield();
    }
}

//==============================================================================
/** */
class BackgroundCaller final : private Thread,
                               private AsyncUpdater
{
public:
    /** */
    BackgroundCaller (std::function<void (void)> func) :
        Thread ("BackgroundCaller"),
        function (func)
    {
        startThread();
    }

    /** */
    ~BackgroundCaller() override
    {
        if (! isBeingDeleted)
            shutdownThreadSafely (*this);
    }

private:
    std::atomic<bool> isBeingDeleted { false };
    std::function<void (void)> function;

    void run() override
    {
        function();
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        isBeingDeleted = true;
        delete this;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BackgroundCaller)
};

/** Calls a function on a background thread. */
inline void callInBackground (std::function<void (void)> function)
{
    new BackgroundCaller (function);
}

//==============================================================================
/** Runs a for-loop that is split between each available core,
    as provided by the thread pool.

    If no thread pool is provided, this will retain the
    for-loop by performing it as per the usual.

    So, this means:
    @code
        for (int i = 0; i < 10; ++i)
    @endcode

    becomes

    @code
        multithreadedFor<int> (0, 10, 1, threadPool, [&] (int i) {});
    @endcode

    @note Make sure each iteration of the loop is independant!
*/
template<typename Type>
inline void multithreadedFor (Type start, Type end, Type interval, ThreadPool* threadPool, std::function<void (Type)> callback)
{
    if (threadPool == nullptr)
    {
        for (int i = start; i < end; i += interval)
            callback (i);

        return;
    }

    const auto num = threadPool->getNumThreads();

    WaitableEvent wait;
    std::atomic<int> threadsRunning (num);

    for (int i = 0; i < num; ++i)
    {
        threadPool->addJob
        ([&]()
        {
            for (int j = start + interval * i; j < end; j += interval * num)
                callback (j);

            const auto stillRunning = --threadsRunning;
            if (stillRunning == 0)
                wait.signal();
        });
    }

    wait.wait();
}

//==============================================================================
/** */
inline void updateOnMessageThread (AsyncUpdater& updater)
{
    if (auto* mm = MessageManager::getInstanceWithoutCreating())
    {
        if (mm->isThisTheMessageThread())
            updater.handleAsyncUpdate();
        else
            updater.triggerAsyncUpdate();
    }
    else
    {
        jassertfalse;
    }
}
