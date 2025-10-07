/** Manually instantiate one of these in a function body to log
    a single function stack crash.

    This is a multi-threaded class, so you can create one of these
    in any function body.

    @warning Do not create a static version of this, or add it as a 
             member. Doing so will likely give false alarms or create
             unfollowable breaks in the logs!

    @see SQUAREPINE_CRASH_TRACER
*/
class CrashStackTracer final
{
public:
    /** Constructor. */
    CrashStackTracer (const char* file, const char* functionName, int line);

    /** Destructor. */
    ~CrashStackTracer();

    //==============================================================================
    /** Dumps the entire known crash stack.

        You can use this in conjunction with juce::SystemStats::setApplicationCrashHandler
        to automatically trigger a log on application crash.

        @code
            SystemStats::setApplicationCrashHandler (CrashStackTracer::dump);
        @endcode
    */
    static void dump (void*);

private:
    //==============================================================================
    friend class GlobalCrashTracer;

    //==============================================================================
    const char* file;
    const char* function;
    const int line;
    Thread::ThreadID threadID;
    String threadName;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CrashStackTracer)
};

//==============================================================================
/** Instantiates a CrashStackTracer, populated with the line and function
    that the macro is called on.

    This is a multi-threaded system, so you can add this to any function body.

    @see CrashStackTracer
*/
#define SQUAREPINE_CRASH_TRACER \
    const CrashStackTracer JUCE_JOIN_MACRO (crashTracer, __LINE__) (__FILE__, __FUNCTION__, __LINE__);
