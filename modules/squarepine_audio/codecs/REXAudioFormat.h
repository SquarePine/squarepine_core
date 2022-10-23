//==============================================================================
#if ! (JUCE_WINDOWS || JUCE_MAC)
    #undef SQUAREPINE_USE_REX_AUDIO_FORMAT
#endif

//==============================================================================
#if SQUAREPINE_USE_REX_AUDIO_FORMAT

/**

    @tags{Audio}
*/
class REXAudioFormat final : public AudioFormat
{
public:
    /** */
    REXAudioFormat (const File& rexLibraryDLL);

    //==============================================================================
    /** @returns Empty if all is fine! */
    String getErrorMessage() const;

    //==============================================================================
    /** Represents the file extension for the current platform, without the prefixed period.

        On macOS, this is "framework".
        On Windows, this is "dll".
    */
    static const char* const rexNativeDLLFileExtension;

    /** This is simply "REX Shared Library". */
    static const char* const rexDLLFilenameWithoutExtension;

    /** Represents the testing version of REX for the current platform.

        This is effectively "REX Shared Library Debugging.(native DLL extension)".
    */
    static const char* const rexDebuggingDLLFilename;

    /** Represents the testing version of REX for the current platform.

        This is effectively "REX Shared Library Testing.(native DLL extension)".
    */
    static const char* const rexTestingDLLFilename;

    /** Represents the testing version of REX for the current platform.

        This is effectively "REX Shared Library.(native DLL extension)".
    */
    static const char* const rexReleaseDLLFilename;

    //==============================================================================
    static const char* const rexName;           /**< Is of type String. */
    static const char* const rexCopyright;      /**< Is of type String. */
    static const char* const rexEmail;          /**< Is of type String. */
    static const char* const rexURL;            /**< Is of type String. */
    static const char* const rexAdditionalText; /**< Is of type String. */
    static const char* const rexTempo;          /**< Is of type double. */
    static const char* const rexNumerator;      /**< Is of type int. */
    static const char* const rexDenominator;    /**< Is of type int. */
    static const char* const rexBeatPoints;     /**< Semicolon (';') separated beat points in samples, and so of type int64. */

    //==============================================================================
    /** @internal */
    Array<int> getPossibleSampleRates() override { return {}; }
    /** @internal */
    Array<int> getPossibleBitDepths() override { return {}; }
    /** @internal */
    bool canDoStereo() override{ return true; }
    /** @internal */
    bool canDoMono() override{ return true; }
    /** @internal */
    bool isCompressed() override{ return true; }
    /** @internal */
    AudioFormatReader* createReaderFor (InputStream*, bool) override;
    /** @internal */
    AudioFormatWriter* createWriterFor (OutputStream*, double, unsignedint, int, const StringPairArray&, int) override { return nullptr; }

private:
    //==============================================================================
    class Reader;
    class REXHandleInstance;
    class REXSystem;

    struct REXSystemDeleter
    {
        void operator() (REXSystem* rs);
    };

    const File rexLibraryDLL;
    std::unique_ptr<REXSystem, REXSystemDeleter> rexSystem;

    //==============================================================================
    bool isREXSystemLoaded() const;
    void reloadREXSystemIfNeeded();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (REXAudioFormat)
};

#endif // SQUAREPINE_USE_REX_AUDIO_FORMAT
