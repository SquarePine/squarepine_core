/** Ensures that all logs that are sent are managed by the main thread/message thread.

    In short, this makes it so file input for the logs gets there safely.
    Also, there's no guarantee that OS and IDE log windows support multithreaded logging.
*/
class MainThreadLogger final : public Logger
{
public:
    /** Constructs a main thread logger.

        @param destinationFile The destination file to use for storing the logs,
                               if the file is valid and can be created
                               (if it's not already).
    */
    MainThreadLogger (const File& destinationFile = {});

    //==============================================================================
    /** A filter for logs.

        @note The larger the value, the more focused the filter.
    */
    enum class Filter
    {
        debug,
        information,
        warnings,
        errors
    };

    /** Changes the currently set filter. */
    void setFilter (Filter);

    /** @returns the currently set filter. */
    Filter getFilter() const;

    /** Logs a message with a filter.

        If the given filter to use is lesser than the set filter,
        the message will not be logged.
    */
    void logMessage (Filter filterToUse, const String&);

    //==============================================================================
    /** @internal */
    void logMessage (const String&) override;

private:
    //==============================================================================
    const File logFile;
    std::atomic<Filter> filter
    {
       #if SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS
        #if JUCE_DEBUG
         Filter::debug
        #else
         Filter::warnings
        #endif
       #else
        Filter::debug
       #endif
    };

    //==============================================================================
    /** Occupies itself with properly outputting to the following destinations:
        - Command line interfaces (cerr and cout, as appropriate).
        - The destination log file.
        - The IDE's console window.
    */
    static void logInternal (File dest, String message, Filter);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainThreadLogger)
};

//==============================================================================
/** */
void logDebug (const String& message);

/** */
void logInfo (const String& message);

/** */
void logWarning (const String& message);

/** */
void logError (const String& message);
