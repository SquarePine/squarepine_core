namespace XMLAttributeKeys
{
    static const String IDKey = "IDValue";
    static const String formatKey = "formatValue";
    static const String fileKey = "fileValue";
}

ChildProcessPluginScanner::ChildProcessPluginScanner() { }

bool ChildProcessPluginScanner::findPluginTypesFor (AudioPluginFormat& format,
                                                    OwnedArray<PluginDescription>& result,
                                                    const String& fileOrIdentifier)
{
    TemporaryFile tempFile;
    auto tempResultsFile = tempFile.getFile();

    StringArray args;
    args.add (File::getSpecialLocation (File::currentExecutableFile).getFullPathName());
    args.add (createCommandArgument (format, fileOrIdentifier, tempResultsFile));

    {
        ChildProcess child;
        if (child.start (args))
        {
            waitForChildProcessOutput (child);

            //For some reason this stops some plugins from hanging, 
            //letting them write their results file.
            if (tempResultsFile.getSize() == 0)
                child.readAllProcessOutput();
        }
    }

    return handleResultsFile (tempResultsFile, result);
}

void ChildProcessPluginScanner::performScan (const String& commandLine, OwnedArray<AudioPluginFormat> customFormats)
{
    const auto request = XmlDocument::parse (commandLine.trim().unquoted());

    if (request != nullptr
        && request->hasTagName ("PluginScan")
        && request->hasAttribute (XMLAttributeKeys::IDKey)
        && request->hasAttribute (XMLAttributeKeys::formatKey)
        && request->hasAttribute (XMLAttributeKeys::fileKey))
    {
        performScanInChildProcess (request->getStringAttribute (XMLAttributeKeys::IDKey),
                                   request->getStringAttribute (XMLAttributeKeys::formatKey),
                                   request->getStringAttribute (XMLAttributeKeys::fileKey),
                                   customFormats);
        return;
    }

    jassertfalse;
}

bool ChildProcessPluginScanner::shouldScan (const String& commandLine)
{
    return commandLine.contains ("PluginScan");
}

void ChildProcessPluginScanner::waitForChildProcessOutput (ChildProcess& child)
{
    int total = 30000;

    while (total > 0)
    {
        if (! MessageManager::getInstance()->isThisTheMessageThread())
        {
            Thread* t = Thread::getCurrentThread();
            if (t == nullptr || t->threadShouldExit())
                break;
        }

        child.waitForProcessToFinish (50);

        Thread::sleep (1);
        total -= 50;
    }
}

bool ChildProcessPluginScanner::handleResultsFile (const File& resultsFile, OwnedArray <PluginDescription>& result)
{
    const auto xml = XmlDocument::parse (resultsFile);
    resultsFile.deleteFile(); //No longer needed, so clean it up

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
    {
        for (auto* e : xml.getChildIterator())
        {
            PluginDescription desc;
            if (desc.loadFromXml (*e))
                found.add (new PluginDescription (desc));
        }
    }
}

String ChildProcessPluginScanner::createCommandArgument (AudioPluginFormat& format,
                                                         const String& fileOrIdentifier,
                                                         const File& tempResultsFile)
{
    XmlElement request ("PluginScan");
    request.setAttribute (XMLAttributeKeys::IDKey, fileOrIdentifier);
    request.setAttribute (XMLAttributeKeys::formatKey, format.getName());
    request.setAttribute (XMLAttributeKeys::fileKey, tempResultsFile.getFullPathName());

    return URL::addEscapeChars (request.toString(), true);
}

void ChildProcessPluginScanner::performScanInChildProcess (const String& fileOrIdentifier, 
                                                           const String& formatName, 
                                                           File resultsFile,
                                                           OwnedArray<AudioPluginFormat>& customFormats)
{
   #if JUCE_MAC
    setupSignalHandling();
   #endif

    if (resultsFile.existsAsFile())
        resultsFile.deleteFile();

    resultsFile.create();
    auto outStream = resultsFile.createOutputStream();

    if (outStream != nullptr && outStream->openedOk())
    {
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

                *outStream << result.toString();
                outStream->flush();
            }
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
