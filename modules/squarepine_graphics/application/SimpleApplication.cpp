SimpleApplication::SimpleApplication()
{
    SystemStats::setApplicationCrashHandler (&sp::CrashStackTracer::dump);
    initSystemRandom();
    initOperatingSystemDependantSystems();
}

SimpleApplication::~SimpleApplication()
{
    Logger::writeToLog ("*** Shutting Down Application ***");
    disableLogging();
}

//==============================================================================
void SimpleApplication::setMainCommandTarget (ApplicationCommandTarget* target)
{
    mainCommandTarget = target;
}

//==============================================================================
void SimpleApplication::initialise (const String& commandLine)
{
    SQUAREPINE_CRASH_TRACER

    if (handleInternalCommandLineOperations (commandLine))
        return;

    enableLogging();
    Logger::writeToLog ("*** Starting Application ***");

    String stats;
    stats
    << "---------------------------" << newLine
    << "=== Operating System ===" << newLine
    << "Name: " << SystemStats::getOperatingSystemName() << newLine
    << "Is OS 64-bit? " << booleanToString (SystemStats::isOperatingSystem64Bit(), true) << newLine
    << "RAM (MB): " << String (SystemStats::getMemorySizeInMegabytes()) << newLine

    << "---------------------------" << newLine
    << "=== User Information ===" << newLine
    << "Display Language: " << SystemStats::getDisplayLanguage() << newLine
    << "User Region: " << SystemStats::getUserRegion() << newLine
    << "Device Description: " << SystemStats::getDeviceDescription() << newLine

    << "---------------------------" << newLine
    << "=== CPU Info ===" << newLine
    << "CPU Model: " << SystemStats::getCpuModel() << newLine
    << "CPU Vendor: " << SystemStats::getCpuVendor() << newLine
    << "CPU Speed (MHz): " << String (SystemStats::getCpuSpeedInMegahertz()) << newLine
    << "Num CPUs: " << String (SystemStats::getNumCpus()) << newLine
    << "Num Physical CPUs: " << String (SystemStats::getNumPhysicalCpus()) << newLine
    << "MMX: " << booleanToString (SystemStats::hasMMX(), true) << newLine
    << "3DNow: " << booleanToString (SystemStats::has3DNow(), true) << newLine
    << "FMA3: " << booleanToString (SystemStats::hasFMA3(), true) << newLine
    << "FMA4: " << booleanToString (SystemStats::hasFMA4(), true) << newLine
    << "SSE: " << booleanToString (SystemStats::hasSSE(), true) << newLine
    << "SSE 2: " << booleanToString (SystemStats::hasSSE2(), true) << newLine
    << "SSE 3: " << booleanToString (SystemStats::hasSSE3(), true) << newLine
    << "SSSE 3: " << booleanToString (SystemStats::hasSSSE3(), true) << newLine
    << "SSE 4.1: " << booleanToString (SystemStats::hasSSE41(), true) << newLine
    << "SSE 4.2: " << booleanToString (SystemStats::hasSSE42(), true) << newLine
    << "AVX: " << booleanToString (SystemStats::hasAVX(), true) << newLine
    << "AVX2: " << booleanToString (SystemStats::hasAVX2(), true) << newLine
    << "AVX512F: " << booleanToString (SystemStats::hasAVX512F(), true) << newLine
    << "AVX512BW: " << booleanToString (SystemStats::hasAVX512BW(), true) << newLine
    << "AVX512CD: " << booleanToString (SystemStats::hasAVX512CD(), true) << newLine
    << "AVX512DQ: " << booleanToString (SystemStats::hasAVX512DQ(), true) << newLine
    << "AVX512ER: " << booleanToString (SystemStats::hasAVX512ER(), true) << newLine
    << "AVX512IFMA: " << booleanToString (SystemStats::hasAVX512IFMA(), true) << newLine
    << "AVX512PF: " << booleanToString (SystemStats::hasAVX512PF(), true) << newLine
    << "AVX512VBMI: " << booleanToString (SystemStats::hasAVX512VBMI(), true) << newLine
    << "AVX512VL: " << booleanToString (SystemStats::hasAVX512VL(), true) << newLine
    << "AVX512VPOPCNTDQ: " << booleanToString (SystemStats::hasAVX512VPOPCNTDQ(), true) << newLine
    << "Neon: " << booleanToString (SystemStats::hasNeon(), true) << newLine

    << "---------------------------" << newLine;

    Logger::writeToLog (stats);

    mainWindow.reset (createWindow());
}

void SimpleApplication::shutdown()
{
    SQUAREPINE_CRASH_TRACER

    mainWindow = nullptr;
}

void SimpleApplication::anotherInstanceStarted (const String& commandLine)
{
    SQUAREPINE_CRASH_TRACER

    commandLineArguments = commandLine;

    handleInternalCommandLineOperations (commandLineArguments);
}

//==============================================================================
bool SimpleApplication::handleInternalCommandLineOperations (const String& commandLine)
{
    return handleUnitTests (commandLine);
}

static void appendSeparatorIfNotThere (String& s)
{
    const auto separator = File::getSeparatorChar();
    if (! s.endsWithChar (separator))
        s << separator;
}

String SimpleApplication::getLoggerFilePath() const
{
    auto logFilePath = File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName();

   #if JUCE_MAC
    appendSeparatorIfNotThere (logFilePath);
    logFilePath << "Application Support";
   #endif

    appendSeparatorIfNotThere (logFilePath);
    logFilePath << const_cast<SimpleApplication*> (this)->getApplicationName();

    appendSeparatorIfNotThere (logFilePath);
    logFilePath << "Logs";

    appendSeparatorIfNotThere (logFilePath);

    return logFilePath;
}

//==============================================================================
void SimpleApplication::initSystemRandom()
{
    std::srand ((uint32) std::time (nullptr));
}

void SimpleApplication::initOperatingSystemDependantSystems()
{
   #if JUCE_LINUX
    Thread::setCurrentThreadName (getApplicationName());
   #endif
}

//==============================================================================
void SimpleApplication::enableLogging()
{
    const auto projectName = getApplicationName();
    auto filename = projectName;

   #if JUCE_DEBUG
    filename << "_debug";
   #endif

    logger.reset (FileLogger::createDateStampedLogger (getLoggerFilePath(), filename, ".txt", projectName + NewLine() + getApplicationVersion()));
    Logger::setCurrentLogger (logger.get());
}

void SimpleApplication::disableLogging()
{
    Logger::setCurrentLogger (nullptr);
    logger = nullptr;
}

//==============================================================================
bool SimpleApplication::handleUnitTests (const String& commandLine)
{
    if (commandLine.contains ("--unit-tests"))
    {
        unitTestRunner.setAbortingTests (false);
        unitTestRunner.runAllTests();

        for (int i = 0; i < unitTestRunner.getNumResults(); ++i)
        {
            if (unitTestRunner.getResult (i)->failures > 0)
            {
                setApplicationReturnValue (1);
                break;
            }
        }

        quit();
        return true;
    }
    else if (commandLine.contains ("--unit-tests-abort"))
    {
        unitTestRunner.setAbortingTests (true);
        quit();
        return true;
    }

    return false;
}
