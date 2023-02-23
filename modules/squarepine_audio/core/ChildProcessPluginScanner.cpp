namespace scanner
{
    #undef DECLARE_ID
    #define DECLARE_ID(x) \
        static const auto x##Id = #x;

    DECLARE_ID (fileOrIdentifier)
    DECLARE_ID (formatName)
    DECLARE_ID (tempFile)

    #undef DECLARE_ID
}

bool ChildProcessPluginScanner::findPluginTypesFor (AudioPluginFormat& format,
                                                    OwnedArray<PluginDescription>& result,
                                                    const String& fileOrIdentifier)
{
    StringArray args;
    args.add (File::getSpecialLocation (File::currentExecutableFile).getFullPathName());

    auto* object = new DynamicObject();
    auto addArg = [&] (const Identifier& id, const String& s)
    {
        object->setProperty (id, URL::addEscapeChars (s, true));
    };

    TemporaryFile tempFile;
    auto tempResultsFile = tempFile.getFile();

    addArg (scanner::fileOrIdentifierId, fileOrIdentifier);
    addArg (scanner::formatNameId, format.getName());
    addArg (scanner::tempFileId, tempResultsFile.getFullPathName());

    args.add (Base64::toBase64 (JSON::toString (object)));

    DBG (args.joinIntoString (" "));

    {
        ChildProcess child;
        if (child.start (args))
        {
            waitForChildProcessOutput (child);

            // For some reason this stops some plugins from hanging, 
            // letting them write their results file.
            if (tempResultsFile.getSize() == 0)
                child.readAllProcessOutput();
        }
    }

    return handleResultsFile (tempResultsFile, result);
}

String ChildProcessPluginScanner::getCommandLineArg (const String& commandLine)
{
    const auto toks = StringArray::fromTokens (commandLine, " ", "\"");

    MemoryOutputStream textStream;
    int index = 0;

    if (toks.size() > 1)
        ++index;

    if (! Base64::convertFromBase64 (textStream, toks[index]))
        return {};

    return textStream.toString();
}

bool ChildProcessPluginScanner::performScan (const String& commandLine, OwnedArray<AudioPluginFormat> customFormats)
{
    const auto jsonData = getCommandLineArg (commandLine);
    if (jsonData.isEmpty())
        return false;

    const auto v = JSON::fromString (jsonData);
    if (auto* object = v.getDynamicObject())
    {
        auto parse = [&] (const Identifier& id)
        {
            return URL::removeEscapeChars (object->getProperty (id).toString());
        };

        performScanInChildProcess (parse (scanner::fileOrIdentifierId),
                                   parse (scanner::formatNameId),
                                   File (parse (scanner::tempFileId)),
                                   customFormats);
        return true;
    }

    return false;
}

bool ChildProcessPluginScanner::canScan (const String& commandLine)
{
    return getCommandLineArg (commandLine).isNotEmpty();
}

void ChildProcessPluginScanner::waitForChildProcessOutput (ChildProcess& child)
{
    int total = 30000;

    while (total > 0)
    {
        if (! MessageManager::getInstance()->isThisTheMessageThread())
        {
            auto* t = Thread::getCurrentThread();
            if (t == nullptr || t->threadShouldExit())
                break;
        }

        child.waitForProcessToFinish (50);

        Thread::sleep (1);
        total -= 50;
    }
}

bool ChildProcessPluginScanner::handleResultsFile (const File& resultsFile, OwnedArray<PluginDescription>& result)
{
    const auto xml = XmlDocument::parse (resultsFile);
    resultsFile.deleteFile(); // No longer needed, so clean it up

    if (xml != nullptr)
    {
        handleResultXml (*xml, result);
        return true;
    }

    return false;
}

void ChildProcessPluginScanner::handleResultXml (const XmlElement& xml, OwnedArray<PluginDescription>& found)
{
    if (xml.hasTagName ("PluginsFound"))
        for (auto* e : xml.getChildIterator())
            if (PluginDescription desc; desc.loadFromXml (*e))
                found.add (new PluginDescription (desc));
}

void ChildProcessPluginScanner::performScanInChildProcess (const String& fileOrIdentifier, 
                                                           const String& formatName, 
                                                           File resultsFile,
                                                           OwnedArray<AudioPluginFormat>& customFormats)
{
   #if JUCE_MAC
    setupSignalHandling();
   #endif

    resultsFile.deleteFile();
    resultsFile.create();
    FileOutputStream outStream (resultsFile);
    if (outStream.failedToOpen())
    {
        JUCEApplication::getInstance()->setApplicationReturnValue (EXIT_FAILURE);
        return;
    }

    AudioPluginFormatManager pluginFormatManager;
    pluginFormatManager.addDefaultFormats();

    for (auto i = customFormats.size(); --i >= 0;)
        pluginFormatManager.addFormat (customFormats.removeAndReturn (i));

    for (auto i = 0; i < pluginFormatManager.getNumFormats(); ++i)
    {
        auto format = pluginFormatManager.getFormat (i);

        if (format->getName() == formatName)
        {
            OwnedArray<PluginDescription> found;
            format->findAllTypesForFile (found, fileOrIdentifier);

            XmlElement result ("PluginsFound");

            for (auto pd : found)
                result.addChildElement (pd->createXml().release());

            outStream << result.toString();
            outStream.flush();
        }
    }
}

#if ! JUCE_WINDOWS

void ChildProcessPluginScanner::killWithoutMercy (int)
{
    std::exit (EXIT_FAILURE);
}

void ChildProcessPluginScanner::setupSignalHandling()
{
/*
    for (auto v : { SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT })
    {
        ::signal (v, killWithoutMercy);
        siginterrupt (v, 1);
    }
*/
}

#endif
