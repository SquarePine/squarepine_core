/** */
class ChildProcessPluginScanner final : public KnownPluginList::CustomScanner
{
public:
    /** */
    ChildProcessPluginScanner() = default;

    //==============================================================================
    /** */
    static bool performScan (const String& commandLine, OwnedArray<AudioPluginFormat> customFormats = {});

    /** */
    [[nodiscard]] static bool canScan (const String& commandLine);

    //==============================================================================
    /** @internal */
    [[nodiscard]] bool findPluginTypesFor (AudioPluginFormat&, OwnedArray<PluginDescription>&, const String&) override;

private:
    //==============================================================================
    static String getCommandLineArg (const String& commandLine);
    void waitForChildProcessOutput (ChildProcess&);
    bool handleResultsFile (const File& resultsFile, OwnedArray<PluginDescription>& result);
    void handleResultXml (const XmlElement& xml, OwnedArray<PluginDescription>& found);

    [[nodiscard]] static String createCommandArgument (AudioPluginFormat&,
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
