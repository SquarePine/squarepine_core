MainThreadLogger::MainThreadLogger (const File& dest) :
    logFile (dest)
{
    if (logFile != File() && ! logFile.existsAsFile())
    {
        logFile.deleteRecursively (true);
        const auto result = logFile.create();
        jassertquiet (result.wasOk());
    }
}

//==============================================================================
void MainThreadLogger::setFilter (Filter newFilter)
{
    filter.store (newFilter, std::memory_order_relaxed);
}

MainThreadLogger::Filter MainThreadLogger::getFilter() const
{
    return filter.load (std::memory_order_relaxed);
}

//==============================================================================
void MainThreadLogger::logMessage (Filter filterToUse, const String& message)
{
    const auto filterSource = (int) getFilter();
    const auto filterDest = (int) filterToUse;

    if (message.isNotEmpty() && filterDest >= filterSource)
        logMessage (message);
}

void MainThreadLogger::logMessage (const String& message)
{
    const auto localMessage = Time::getCurrentTime().toISO8601 (true) + " " + message;
    const auto localFilter = getFilter();

    if (auto* mm = MessageManager::getInstanceWithoutCreating())
    {
        if (mm->isThisTheMessageThread())
        {
            logInternal (logFile, localMessage, localFilter);
        }
        else
        {
            MessageManager::callAsync ([logFileDupe = logFile, localMessage, localFilter]()
            {
                logInternal (logFileDupe, localMessage, localFilter);
            });
        }
    }
    else
    {
        logInternal (logFile, localMessage, localFilter);
    }
}

void MainThreadLogger::logInternal (File dest, String message, Filter filterToUse)
{
    if (! message.endsWith (newLine))
        message << newLine;

    if (message.containsIgnoreCase ("error") || filterToUse == Filter::errors)
        std::cerr << message;
    else
        std::cout << message;

    outputDebugString (message);

    if (dest != File())
    {
        const bool b = dest.appendText (message);
        jassertquiet (b);
    }
}

//==============================================================================
inline void logWithFilter (MainThreadLogger::Filter filter, const String& tagName, const String& message)
{
    auto* currentLogger = dynamic_cast<MainThreadLogger*> (Logger::getCurrentLogger());

    /** You seem to be trying to log with something other than the MainThreadLogger.
        Please call Logger::setCurrentLogger() before doing so!
    */
    jassert (currentLogger != nullptr);

    if (currentLogger != nullptr)
        currentLogger->logMessage (filter, tagName + ": " + message);
}

void logDebug (const String& message)
{
    logWithFilter (MainThreadLogger::Filter::debug, "DEBUG", message);
}

void logInfo (const String& message)
{
    logWithFilter (MainThreadLogger::Filter::information, "INFO", message);
}

void logWarning (const String& message)
{
    logWithFilter (MainThreadLogger::Filter::warnings, "WARNING", message);
}

void logError (const String& message)
{
    logWithFilter (MainThreadLogger::Filter::errors, "ERROR", message);
}
