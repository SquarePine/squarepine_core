/** */
class ChildProcessPluginScanner final : public KnownPluginList::CustomScanner
{
public:
    /** */
    ChildProcessPluginScanner();

    //==============================================================================
    /** */
    static void performScan (const String& commandLine, OwnedArray<AudioPluginFormat> customFormats = {});

    /** */
    static [[nodiscard]] bool shouldScan (const String& commandLine);

    //==============================================================================
    /** @internal */
    [[nodiscard]] bool findPluginTypesFor (AudioPluginFormat&, OwnedArray<PluginDescription>&, const String&) override;

private:
    //==============================================================================
    void waitForChildProcessOutput (ChildProcess& child);
    bool handleResultsFile (const File& resultsFile, OwnedArray<PluginDescription>& result);
    void handleResultXml (const XmlElement& xml, OwnedArray<PluginDescription>& found);

    static [[nodiscard]] String createCommandArgument (AudioPluginFormat&,
                                                       const String& fileOrIdentifier,
                                                       const File& tempResultsFile);

    static void performScanInChildProcess (const String& fileOrIdentifier, 
                                           const String& formatName, 
                                           File resultsFile,
                                           OwnedArray<AudioPluginFormat>& customFormats);

    //==============================================================================
   #if ! JUCE_WINDOWS
    static void killWithoutMercy (int);
    static void setupSignalHandling();
   #endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChildProcessPluginScanner)
};
