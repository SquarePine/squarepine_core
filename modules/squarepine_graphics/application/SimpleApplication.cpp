SimpleApplication::SimpleApplication()
{
    SystemStats::setApplicationCrashHandler (&sp::CrashStackTracer::dump);
    std::srand ((uint32) std::time (nullptr));
}

SimpleApplication::~SimpleApplication()
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog ("*** Shutting Down Application ***");
    std::cout << std::flush;
    std::cerr << std::flush;
    Logger::setCurrentLogger (nullptr);
    logger = nullptr;
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
    {
        quit();
        return;
    }

   #if JUCE_LINUX
    Thread::setCurrentThreadName (getApplicationName());
   #endif

    enableLogging();
    Logger::writeToLog ("*** Starting Application ***");

    const auto separator = String::repeatedString ("-", 30);

    auto createLogTitle = [] (const String& name) -> String
    {
        return "=== " + name + " ===";
    };

    String stats;
    stats.preallocateBytes (512);

    stats
        << separator << newLine
        << createLogTitle ("Operating System") << newLine
        << "Name: " << SystemStats::getOperatingSystemName() << newLine
       #if JUCE_ANDROID
        << "Android Release: " << getAndroidReleaseVersion() << newLine
        << "Android SDK: " << getAndroidSDKVersion() << newLine
       #if __ANDROID_API__ >= 24
        << "Android Target SDK: " << android_get_application_target_sdk_version() << newLine
       #endif
       #endif
        << "Is OS 64-bit? " << booleanToString (SystemStats::isOperatingSystem64Bit(), true) << newLine
        << "Memory: " << SystemStats::getMemorySizeInMegabytes() << " MiB" << newLine
        << newLine
        << createLogTitle ("User") << newLine
        << "User Language: " << SystemStats::getUserLanguage() << newLine
        << "User Region: " << SystemStats::getUserRegion() << newLine
        << "Display Language: " << SystemStats::getDisplayLanguage() << newLine
        << "Device Description: " << SystemStats::getDeviceDescription() << newLine
        << "Device Manufacturer: " << SystemStats::getDeviceManufacturer() << newLine
        << newLine
        << createLogTitle ("CPU") << newLine
        << "CPU Model: " << SystemStats::getCpuModel() << newLine
        << "CPU Vendor: " << SystemStats::getCpuVendor() << newLine
        << "CPU Speed: " << SystemStats::getCpuSpeedInMegahertz() << " MHz" << newLine
        << "Num CPUs: " << SystemStats::getNumCpus() << newLine
        << "Num Physical CPUs: " << SystemStats::getNumPhysicalCpus() << newLine
        << "CPU Features: " << newLine;

    auto appendIfTrue = [&] (const String& name, bool b)
    {
        if (b)
            stats << "\t- " << name << newLine;
    };

    appendIfTrue ("MMX",                SystemStats::hasMMX());
    appendIfTrue ("3DNow",              SystemStats::has3DNow());
    appendIfTrue ("FMA3",               SystemStats::hasFMA3());
    appendIfTrue ("FMA4",               SystemStats::hasFMA4());
    appendIfTrue ("SSE",                SystemStats::hasSSE());
    appendIfTrue ("SSE 2",              SystemStats::hasSSE2());
    appendIfTrue ("SSE 3",              SystemStats::hasSSE3());
    appendIfTrue ("SSSE 3",             SystemStats::hasSSSE3());
    appendIfTrue ("SSE 4.1",            SystemStats::hasSSE41());
    appendIfTrue ("SSE 4.2",            SystemStats::hasSSE42());
    appendIfTrue ("AVX",                SystemStats::hasAVX());
    appendIfTrue ("AVX2",               SystemStats::hasAVX2());
    appendIfTrue ("AVX512F",            SystemStats::hasAVX512F());
    appendIfTrue ("AVX512BW",           SystemStats::hasAVX512BW());
    appendIfTrue ("AVX512CD",           SystemStats::hasAVX512CD());
    appendIfTrue ("AVX512DQ",           SystemStats::hasAVX512DQ());
    appendIfTrue ("AVX512ER",           SystemStats::hasAVX512ER());
    appendIfTrue ("AVX512IFMA",         SystemStats::hasAVX512IFMA());
    appendIfTrue ("AVX512PF",           SystemStats::hasAVX512PF());
    appendIfTrue ("AVX512VBMI",         SystemStats::hasAVX512VBMI());
    appendIfTrue ("AVX512VL",           SystemStats::hasAVX512VL());
    appendIfTrue ("AVX512VPOPCNTDQ",    SystemStats::hasAVX512VPOPCNTDQ());
    appendIfTrue ("Neon",               SystemStats::hasNeon());

    stats << separator << newLine << newLine;
    Logger::writeToLog (stats);

    mainWindow = createWindow();
}

void SimpleApplication::shutdown()
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog ("Shutdown was called.");
    mainWindow = nullptr;
}

void SimpleApplication::anotherInstanceStarted (const String& commandLine)
{
    SQUAREPINE_CRASH_TRACER
    Logger::writeToLog ("Started new instance of application...");

    commandLineArguments = commandLine;

    handleInternalCommandLineOperations (commandLineArguments);
}

void SimpleApplication::suspended()
{
    SQUAREPINE_CRASH_TRACER
    Logger::writeToLog ("Application was suspended.");
}

void SimpleApplication::resumed()
{
    SQUAREPINE_CRASH_TRACER
    Logger::writeToLog ("Application was resumed.");
}

void SimpleApplication::unhandledException (const std::exception* e,
                                            const String& sourceFilename,
                                            int lineNumber)
{
    SQUAREPINE_CRASH_TRACER

    String message;
    message
        << "ERROR --- received unhandled std::exception!" << newLine
        << "Line Number: " << lineNumber << newLine
        << "Source Filename: " << sourceFilename << newLine;

    if (e != nullptr)
        message << "What: " << e->what() << newLine;

    Logger::writeToLog (message);
    jassertfalse;
}

void SimpleApplication::memoryWarningReceived()
{
    SQUAREPINE_CRASH_TRACER
    Logger::writeToLog ("Received memory warning from OS!");
    jassertfalse;
}

bool SimpleApplication::backButtonPressed()
{
    SQUAREPINE_CRASH_TRACER
    Logger::writeToLog ("Back button was pressed.");
    return false;
}

//==============================================================================
bool SimpleApplication::handleInternalCommandLineOperations (const String& args)
{
    SQUAREPINE_CRASH_TRACER
    return handleUnitTests (args);
}

String SimpleApplication::getLoggerFilePath() const
{
    auto appendSeparatorIfNotThere = [] (String& s)
    {
        const auto separator = File::getSeparatorChar();
        if (! s.endsWithChar (separator))
            s << separator;
    };

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

//==============================================================================
bool SimpleApplication::handleUnitTests (const String& args)
{
    if (args.contains ("--unit-tests"))
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

        return true;
    }

    if (args.isNotEmpty())
        Logger::writeToLog (String ("Unknown command line args given: ") + newLine + args);

    return false;
}
