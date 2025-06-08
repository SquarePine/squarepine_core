#if SQUAREPINE_USE_REX_AUDIO_FORMAT

#if JUCE_WINDOWS && ! JUCE_64BIT
    #define REXCALL _cdecl
#else
    #define REXCALL
#endif

#define REX_BUILD_VERSION(major, minor, revision) \
    (major) * 1000000 + (minor) * 1000 + (revision)

enum
{
    REX_API_VERSION                 = REX_BUILD_VERSION (1, 1, 1),
    REX_FIRST_64BIT_VERSION         = REX_BUILD_VERSION (1, 7, 0),
    rexMajorCompatibilityVersion    = REX_API_VERSION / 1000000
};

//==============================================================================
#if JUCE_WINDOWS
    #define REX_DYNLIB_EXTENSION "dll"
#elif JUCE_MAC
    #define REX_DYNLIB_EXTENSION "framework"
#else
    #error "TODO?"
#endif

const char* const REXAudioFormat::rexNativeDLLFileExtension         = REX_DYNLIB_EXTENSION;
const char* const REXAudioFormat::rexDLLFilenameWithoutExtension    = "REX Shared Library";
const char* const REXAudioFormat::rexDebuggingDLLFilename           = "REX Shared Library Debugging." REX_DYNLIB_EXTENSION;
const char* const REXAudioFormat::rexTestingDLLFilename             = "REX Shared Library Testing." REX_DYNLIB_EXTENSION;
const char* const REXAudioFormat::rexReleaseDLLFilename             = "REX Shared Library." REX_DYNLIB_EXTENSION;

#undef REX_DYNLIB_EXTENSION

//==============================================================================
const char* const REXAudioFormat::rexName           = "name";
const char* const REXAudioFormat::rexCopyright      = "copyright";
const char* const REXAudioFormat::rexEmail          = "email";
const char* const REXAudioFormat::rexURL            = "url";
const char* const REXAudioFormat::rexAdditionalText = "additionalText";
const char* const REXAudioFormat::rexTempo          = "tempo";
const char* const REXAudioFormat::rexNumerator      = "numerator";
const char* const REXAudioFormat::rexDenominator    = "denominator";
const char* const REXAudioFormat::rexBeatPoints     = "beatPoints";

//==============================================================================
namespace REXError
{
    enum ErrorCode
    {
        /** Not really errors */
        noError = 1,
        operationAbortedByUser,
        noCreatorInfoAvailable,

        /** Run-time errors */
        notEnoughMemoryForDLL = 100,
        unableToLoadDLL,
        dllTooOld,
        dllNotFound,
        apiTooOld,
        outOfMemory,
        fileCorrupt,
        rex2FileTooNew,
        fileHasZeroLoopLength,
        osVersionNotSupported,

        /** Implementation errors */
        dllNotLoaded = 200,
        dllAlreadyLoaded,
        invalidHandle,
        invalidSize,
        invalidArgument,
        invalidSlice,
        invalidSampleRate,
        bufferTooSmall,
        isBeingPreviewed,
        notBeingPreviewed,
        invalidTempo,

        /** DLL error - call the cops! */
        undefined = 666
    };

    static String rexErrorToString (ErrorCode e)
    {
        static HashMap<int, String> errorList;

        if (errorList.size() <= 0)
        {
            errorList.set (noError,                 NEEDS_TRANS ("REX (code 123): No error."));
            errorList.set (operationAbortedByUser,  NEEDS_TRANS ("REX (code 123): Operation aborted by user."));
            errorList.set (noCreatorInfoAvailable,  NEEDS_TRANS ("REX (code 123): No creator information available."));
            errorList.set (notEnoughMemoryForDLL,   NEEDS_TRANS ("REX (code 123): Not enough memory for the DLL."));
            errorList.set (unableToLoadDLL,         NEEDS_TRANS ("REX (code 123): Unable to load the DLL."));
            errorList.set (dllTooOld,               NEEDS_TRANS ("REX (code 123): DLL is too old."));
            errorList.set (dllNotFound,             NEEDS_TRANS ("REX (code 123): DLL wasn't found."));
            errorList.set (apiTooOld,               NEEDS_TRANS ("REX (code 123): API is too old."));
            errorList.set (outOfMemory,             NEEDS_TRANS ("REX (code 123): Out of memory."));
            errorList.set (fileCorrupt,             NEEDS_TRANS ("REX (code 123): File is corrupt."));
            errorList.set (rex2FileTooNew,          NEEDS_TRANS ("REX (code 123): REX2 file is too new."));
            errorList.set (fileHasZeroLoopLength,   NEEDS_TRANS ("REX (code 123): File has a zero length for the loop."));
            errorList.set (osVersionNotSupported,   NEEDS_TRANS ("REX (code 123): OS version is not supported."));
            errorList.set (dllNotLoaded,            NEEDS_TRANS ("REX (code 123): DLL wasn't loaded."));
            errorList.set (dllAlreadyLoaded,        NEEDS_TRANS ("REX (code 123): DLL is already loaded."));
            errorList.set (invalidHandle,           NEEDS_TRANS ("REX (code 123): Invalid handle."));
            errorList.set (invalidSize,             NEEDS_TRANS ("REX (code 123): Invalid size."));
            errorList.set (invalidArgument,         NEEDS_TRANS ("REX (code 123): Invalid argument."));
            errorList.set (invalidSlice,            NEEDS_TRANS ("REX (code 123): Invalid slice."));
            errorList.set (invalidSampleRate,       NEEDS_TRANS ("REX (code 123): Invalid sample rate."));
            errorList.set (bufferTooSmall,          NEEDS_TRANS ("REX (code 123): Buffer too small."));
            errorList.set (isBeingPreviewed,        NEEDS_TRANS ("REX (code 123): Is being previewed."));
            errorList.set (notBeingPreviewed,       NEEDS_TRANS ("REX (code 123): Not being previewed."));
            errorList.set (invalidTempo,            NEEDS_TRANS ("REX (code 123): Invalid tempo."));
            errorList.set (undefined,               NEEDS_TRANS ("REX (code 123): Undefined!"));
        }

        return TRANS (errorList[e])
               .replace ("123", String ((int) e))
               .trim();
    }

    static String rexErrorToStringVerbose (ErrorCode e)
    {
        switch (e)
        {
            case notEnoughMemoryForDLL:
            case outOfMemory:
                return TRANS ("The Propellerheads REX format requires additional memory and you don't have enough available!");

            case osVersionNotSupported:
                return TRANS ("The Propellerheads REX format requires a more modern operating system!");

            case dllNotFound:
                return TRANS ("The Propellerheads REX format is not installed on your machine!")
                       + "\n\n"
                       + TRANS ("Please navigate to the application settings to install REX support on your system.");

            case unableToLoadDLL:
                return TRANS ("The Propellerheads REX format installed on your machine could not be loaded!")
                       + "\n\n"
                       + TRANS ("For an unknown reason, the library could not be loaded.")
                       + "\n"
                       + TRANS ("Please try using the latest \"REX shared library\" installer from propellerheads.se");

            case dllTooOld:
                return TRANS ("The Propellerheads REX format installed on your machine is outdated!")
                       + "\n\n"
                       + TRANS ("In order to work with rx2 files, please download the latest \"REX shared library\" installer from propellerheads.se");

            case noError:
                return TRANS ("An unknown error occured with the Propellerheads REX format!")
                       + "\n\n"
                       + TRANS ("Please try using the latest \"REX shared library\" installer from propellerheads.se");

            default:
            break;
        };

        return rexErrorToString (e);
    }

    static bool checkRexError (ErrorCode e)
    {
        if (e != noError)
        {
            Logger::writeToLog (rexErrorToString (e));
            return false;
        }

        return true;
    }
}

//==============================================================================
#if JUCE_MSVC
    #pragma pack (push, 8)
#endif

struct REXInfo final
{
    int numChannels = 0, 
        sampleRate = 0,
        numSlices = 0,
        tempo = 0,                      // Tempo set when exported from ReCycle, 123.456 BPM stored as 123456L etc.
        originalTempo = 0,              // Original tempo of loop, as calculated by ReCycle from the locator positions and bars/beats/sign settings.
        ppqLength = 0,
        timeSignatureNumerator = 0,
        timeSignatureDenominator = 0,
        bitDepth = 0;
};

struct REXSliceInfo final
{
    int ppqPos = 0,         // Position of slice in loop
        sampleLength = 0;   // Length of rendered slice, at its original sample rate.
};

struct REXCreatorInfo final
{
    REXCreatorInfo() noexcept { zerostruct (*this); }

    enum { kREXStringSize = 256 };

    char name[kREXStringSize];
    char copyright[kREXStringSize];
    char url[kREXStringSize];
    char email[kREXStringSize];
    char freeText[kREXStringSize];
};

#if JUCE_MSVC
    #pragma pack (pop)
#endif

//==============================================================================
struct REXDLLHandle final
{
    REXDLLHandle (const File& moduleToLoad)
    {
        const auto path = moduleToLoad.getFullPathName().trim();
        if (path.isNotEmpty())
            ok = open (path);
    }

    ~REXDLLHandle()
    {
        close();
    }

    void* getFunction (const char* functionName)
    {
       #if JUCE_WINDOWS
        return library.getFunction (functionName);
       #else
        if (bundleRef == nullptr)
            return nullptr;

        auto name = String (functionName).toCFString();
        void* fn = CFBundleGetFunctionPointerForName (bundleRef, name);
        CFRelease (name);
        return fn;
       #endif
    }

    void close()
    {
       #if JUCE_WINDOWS
        library.close();
       #elif JUCE_MAC
        if (bundleRef != nullptr)
        {
            CFRelease (bundleRef);
            bundleRef = nullptr;
        }
       #endif
    }

    bool ok = false;

private:
   #if JUCE_WINDOWS
    DynamicLibrary library;

    bool open (const String& filePath) { return library.open (filePath); }
   #else
    CFBundleRef bundleRef;

    bool open (const String& filePath)
    {
        const File file (filePath);
        const char* const utf8 = file.getFullPathName().toRawUTF8();

        if (CFURLRef url = CFURLCreateFromFileSystemRepresentation (nullptr, (const UInt8*) utf8, (CFIndex) std::strlen (utf8), file.isDirectory()))
        {
            bundleRef = CFBundleCreate (kCFAllocatorDefault, url);
            CFRelease (url);

            if (bundleRef != nullptr)
            {
                CFErrorRef error = nullptr;

                if (CFBundleLoadExecutableAndReturnError (bundleRef, &error))
                    return true;

                if (error != nullptr)
                {
                    if (CFStringRef failureMessage = CFErrorCopyFailureReason (error))
                    {
                        DBG (String::fromCFString (failureMessage));
                        CFRelease (failureMessage);
                    }

                    CFRelease (error);
                }

                CFRelease (bundleRef);
                bundleRef = nullptr;
            }
        }

        return false;
    }
   #endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (REXDLLHandle)
};

//==============================================================================
using REXHandle = void*;

class REXAudioFormat::REXSystem final
{
public:
    REXSystem (const File& rexLibraryDllOrBundle) :
        rexLibraryFile (rexLibraryDllOrBundle),
        result (REXError::noError)
    {
       #if JUCE_MAC
        if (! rexLibraryFile.exists())
       #else
        if (! rexLibraryFile.existsAsFile())
       #endif
        {
            succeededOrAssert (REXError::unableToLoadDLL);
            return;
        }

        if (! succeededOrAssert (isSupportedOnThisOS (rexLibraryFile) ? REXError::noError : REXError::osVersionNotSupported))
            return;

        if (! succeededOrAssert (checkLibraryVersion (rexLibraryFile)))
            return;

        auto actualREXLibraryFile = rexLibraryFile;

       #if JUCE_MAC
        //Need to load the actual bundle, not the framework:
        actualREXLibraryFile = actualREXLibraryFile.getChildFile ("Versions/A/");
       #endif

        rexLibrary.reset (new REXDLLHandle (actualREXLibraryFile));
        if (! rexLibrary->ok)
        {
            failAndCloseLibrary (REXError::unableToLoadDLL);
            return;
        }

        if (! loadREXFunctions() || dllOpen() == 0)
        {
            failAndCloseLibrary (REXError::unableToLoadDLL);
            return;
        }
    }

    ~REXSystem()                                            { closeLibrary(); }
    bool isOk() const noexcept                              { return result == REXError::noError; }
    String getErrorMessage() const noexcept                 { return rexErrorToStringVerbose (result); }
    REXError::ErrorCode getLastErrorCode() const noexcept   { return result; }

    enum REXCallbackResult
    {
        kREXCallback_Abort = 1,
        kREXCallback_Continue
    };

    typedef REXCallbackResult (REXCALL* REXCreateCallback) (int percentFinished, void* userData);

    #define DEFINE_REX_FUNCTION(returnType, functionName, params) \
        typedef returnType (REXCALL * functionName ## Ptr) params; \
        functionName ## Ptr functionName;

    /** @returns A boolean in integral form. */
    DEFINE_REX_FUNCTION (char, dllOpen, ())

    DEFINE_REX_FUNCTION (void, dllClose, ())
    DEFINE_REX_FUNCTION (REXError::ErrorCode, createHandle, (REXHandle*, const char [], int, REXCreateCallback, void*))
    DEFINE_REX_FUNCTION (void, deleteHandle, (REXHandle*))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, getInformation, (REXHandle, int, REXInfo*))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, getInfoFromBuffer, (int, const char [], int, REXInfo*))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, getCreatorInfo, (REXHandle, int, REXCreatorInfo*))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, getSliceInfo, (REXHandle, int, int, REXSliceInfo*))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, setOutputSampleRate, (REXHandle, int))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, renderSlice, (REXHandle, int, int, float* [2]))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, startPreview, (REXHandle))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, stopPreview, (REXHandle))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, renderPreviewBatch, (REXHandle, int, float* [2]))
    DEFINE_REX_FUNCTION (REXError::ErrorCode, setPreviewTempo, (REXHandle, int))

    #undef DEFINE_REX_FUNCTION

    void createHandleSafely (REXHandle* handle, const char buffer[], int size, REXCreateCallback callback = nullptr, void* userData = nullptr)
    {
        if (! isOk())
            return; //Early return to preserve the last failure message.

        REXError::ErrorCode code = REXError::dllNotLoaded;

        if (createHandle != nullptr)
            code = createHandle (handle, buffer, size, callback, userData);

        succeededOrAssert (code);
    }

    void deleteHandleSafely (REXHandle* handle)
    {
        if (isOk() && deleteHandle != nullptr)
            deleteHandle (handle);
        else
            succeededOrAssert (REXError::dllNotLoaded);
    }

private:
    /** Note that you need to keep a File instance around of the DLL/Bundle to let
        the OS lock it for our application. This way the user can't move it on us,
        which would break everything during runtime.
    */
    const File rexLibraryFile;
    std::unique_ptr<REXDLLHandle> rexLibrary;
    REXError::ErrorCode result;

    void closeLibrary()
    {
        if (dllClose != nullptr)
            dllClose();

        rexLibrary = nullptr;
    }

    void failAndCloseLibrary (REXError::ErrorCode e)
    {
        succeededOrAssert (e); //Just to get the log and assertion...
        closeLibrary();
    }

    static int parseVersion (const String& source, int index)
    {
        auto s = source;

        for (int i = 0; i < std::min (index, 4); ++i)
            s = s.fromFirstOccurrenceOf (".", false, true);

        return s.trim().getIntValue();
    }

    static String getVersion (const File& libraryToCheck)
    {
       #if JUCE_MAC
        if (auto e = XmlDocument::parse (libraryToCheck.getChildFile ("Versions/A/Resources/Info.plist")))
        {
            if (auto* dict = e->getChildByName ("dict"))
            {
                StringPairArray pairs;
                String lastKey;

                for (int i = 0; i < dict->getNumChildElements(); ++i)
                {
                    if (auto* keyOrValue = dict->getChildElement (i))
                    {
                        if (keyOrValue->getTagName().equalsIgnoreCase ("key"))
                        {
                            lastKey = keyOrValue->getAllSubText();
                            pairs.set (lastKey, String());
                        }
                        else
                        {
                            pairs.set (lastKey, keyOrValue->getAllSubText());
                        }
                    }
                }

                return pairs["CFBundleVersion"];
            }
        }

        jassertfalse;
        return {};
       #else
        return libraryToCheck.getVersion();
       #endif
    }

    static REXError::ErrorCode checkLibraryVersion (const File& libraryToCheck)
    {
        const auto v = getVersion (libraryToCheck);
        const auto revision = parseVersion (v, 0);

        if (revision != rexMajorCompatibilityVersion)
        {
            if (revision > rexMajorCompatibilityVersion)
                return REXError::apiTooOld;

            return REXError::dllTooOld;
        }

        const auto version = parseVersion (v, 1);
        if (version < 7)
            return REXError::dllTooOld;

        const auto buildHi = parseVersion (v, 2);
        const auto fullBuildVersion = REX_BUILD_VERSION (revision, version, buildHi);

        if (REX_API_VERSION > fullBuildVersion)
            return REXError::dllTooOld;

       #if JUCE_64BIT
        if (REX_FIRST_64BIT_VERSION > fullBuildVersion)
            return REXError::dllTooOld;
       #endif

        return REXError::noError;
    }

    static bool isProbablyV170 (const File& libraryToCheck) noexcept { return getVersion (libraryToCheck).containsIgnoreCase ("1.7.0"); }

    static bool isSupportedOnThisOS (const File& libraryToCheck)
    {
       #if JUCE_WINDOWS
        if (isProbablyV170 (libraryToCheck))
            return SystemStats::getOperatingSystemType() >= SystemStats::WinXP;

        return SystemStats::getOperatingSystemType() >= SystemStats::Windows7;
       #elif JUCE_MAC
        ignoreUnused (libraryToCheck);
        return SystemStats::getOperatingSystemType() >= SystemStats::MacOSX_10_7;
       #else
        ignoreUnused (libraryToCheck);
        return false;
       #endif
    }

    bool loadREXFunctions()
    {
        if (! isOk())
            return false;

        // "Open/Close" apply to V1.7.0 and below (note that we only support v1.7.0 and up)
        dllOpen = (dllOpenPtr) rexLibrary->getFunction ("Open");
        dllClose = (dllClosePtr) rexLibrary->getFunction ("Close");

        if (isProbablyV170 (rexLibraryFile) && (dllOpen == nullptr || dllClose == nullptr))
        {
            jassertfalse;
            return false;
        }
        else
        {
            // V1.8.0
            dllOpen = (dllOpenPtr) rexLibrary->getFunction ("REXInitializeDLL");
            dllClose = (dllClosePtr) rexLibrary->getFunction ("REXUninitializeDLL");
        }

        // Mandatory functions:
        createHandle        = (createHandlePtr) rexLibrary->getFunction ("REXCreate");
        deleteHandle        = (deleteHandlePtr) rexLibrary->getFunction ("REXDelete");
        getInformation      = (getInformationPtr) rexLibrary->getFunction ("REXGetInfo");
        getInfoFromBuffer   = (getInfoFromBufferPtr) rexLibrary->getFunction ("REXGetInfoFromBuffer");
        getCreatorInfo      = (getCreatorInfoPtr) rexLibrary->getFunction ("REXGetCreatorInfo");
        getSliceInfo        = (getSliceInfoPtr) rexLibrary->getFunction ("REXGetSliceInfo");
        setOutputSampleRate = (setOutputSampleRatePtr) rexLibrary->getFunction ("REXSetOutputSampleRate");
        renderSlice         = (renderSlicePtr) rexLibrary->getFunction ("REXRenderSlice");
        startPreview        = (startPreviewPtr) rexLibrary->getFunction ("REXStartPreview");
        stopPreview         = (stopPreviewPtr) rexLibrary->getFunction ("REXStopPreview");
        renderPreviewBatch  = (renderPreviewBatchPtr) rexLibrary->getFunction ("REXRenderPreviewBatch");
        setPreviewTempo     = (setPreviewTempoPtr) rexLibrary->getFunction ("REXSetPreviewTempo");

        return dllOpen != nullptr
            && dllClose != nullptr
            && createHandle != nullptr
            && deleteHandle != nullptr
            && getInformation != nullptr
            && getInfoFromBuffer != nullptr
            && getCreatorInfo != nullptr
            && getSliceInfo != nullptr
            && setOutputSampleRate != nullptr
            && renderSlice != nullptr
            && startPreview != nullptr
            && stopPreview != nullptr
            && renderPreviewBatch != nullptr
            && setPreviewTempo != nullptr;
    }

    bool succeededOrAssert (REXError::ErrorCode e)
    {
        if (! REXError::checkRexError (e))
        {
            result = e;
            jassertfalse;
            return false;
        }

        return true;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (REXSystem)
};

void REXAudioFormat::REXSystemDeleter::operator() (REXSystem* rs)
{
    deleteAndZero (rs);
}

//==============================================================================
/** Small RAII wrapper around a REXHandle, which automatically deletes the handle when out of scope (if it's valid). */
class REXAudioFormat::REXHandleInstance final
{
public:
    /** @warning Use create() unless you know what you're doing! */
    REXHandleInstance (REXSystem& rs, REXHandle h) :
        rexSystem (rs),
        handle (h)
    {
    }

    ~REXHandleInstance()
    {
        if (handle != nullptr)
            rexSystem.deleteHandleSafely (&handle);
    }

    static std::unique_ptr<REXHandleInstance> create (REXSystem& rexSystem, const void* rexData, size_t rexDataSize)
    {
        REXHandle h = nullptr;
        rexSystem.createHandleSafely (&h, (const char*) rexData, (int) rexDataSize, nullptr, nullptr);
        if (rexSystem.isOk() && h != nullptr)
            return std::make_unique<REXHandleInstance> (rexSystem, h);

        return {};
    }

    REXHandle& get() { jassert (handle != nullptr); return handle; }
    operator REXHandle&() { return get(); }

private:
    REXSystem& rexSystem;
    REXHandle handle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (REXHandleInstance)
};

//==============================================================================
class REXAudioFormat::Reader : public AudioFormatReader
{
public:
    Reader (REXSystem& system, InputStream* sourceStream,  const String& name) :
        AudioFormatReader (sourceStream, name),
        buffer (2, 1024)
    {
        MemoryBlock rexData;
        sourceStream->readIntoMemoryBlock (rexData);

        loadedOk = decompress (system, rexData.getData(), rexData.getSize());
    }

    bool readSamples (int* const* destChannels, int numDestChannels,
                      int startOffsetInDestBuffer, int64 startSampleInFile,
                      int numSamples) override
    {
        jassert (destChannels != nullptr);
        jassert (loadedOk);

        if (destChannels == nullptr || ! loadedOk)
            return false;

        clearSamplesBeyondAvailableLength (destChannels, numDestChannels, startOffsetInDestBuffer,
                                           startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        const auto localNumChannels = safeNumChannels.load();

        if (safeUsesFloatingPointData)
        {
            const auto offset = (size_t) startOffsetInDestBuffer;

            for (int i = numDestChannels; --i >= 0;)
            {
                if (auto* targetChannel = destChannels[i])
                {
                    const float* sourceChannel = nullptr;

                    if (i < localNumChannels)
                        sourceChannel = buffer.getReadPointer (i, (int) startSampleInFile);

                    if (sourceChannel != nullptr)
                        FloatVectorOperations::copy (reinterpret_cast<float*> (targetChannel) + offset, sourceChannel, numSamples);
                    else
                        FloatVectorOperations::clear (reinterpret_cast<float*> (targetChannel) + offset, numSamples);
                }
            }

            return true;
        }

        // Convert from float to int32:
        using DestinationType = AudioData::Pointer<AudioData::Int32, AudioData::LittleEndian, AudioData::NonInterleaved, AudioData::NonConst>;
        using SourceType = AudioData::Pointer<AudioData::Float32, AudioData::LittleEndian, AudioData::NonInterleaved, AudioData::Const>;

        for (int i = numDestChannels; --i >= 0;)
        {
            if (auto* targetChannel = destChannels[i])
            {
                DestinationType dest (targetChannel);
                dest += startOffsetInDestBuffer;

                const float* sourceChannel = nullptr;

                if (i < localNumChannels)
                    sourceChannel = buffer.getReadPointer (i, (int) startSampleInFile);

                if (sourceChannel != nullptr)
                    dest.convertSamples (SourceType (sourceChannel), numSamples);
                else
                    dest.clearSamples (numSamples);
            }
        }

        return true;
    }

    bool loadedOk;

private:
    AudioBuffer<float> buffer;
    std::atomic<int> safeNumChannels { 0 };
    const std::atomic<bool> safeUsesFloatingPointData { true };

    bool decompress (REXSystem& rs, const void* rexData, size_t rexDataSize)
    {
        auto handle = REXHandleInstance::create (rs, rexData, rexDataSize);
        if (handle == nullptr)
            return false;

        REXInfo info;
        if (rs.getInformation == nullptr
            || ! checkRexError (rs.getInformation (handle->get(), sizeof (info), &info)))
        {
            jassertfalse;
            return false;
        }

        if (rs.setOutputSampleRate == nullptr
            || ! checkRexError (rs.setOutputSampleRate (handle->get(), info.sampleRate)))
        {
            jassertfalse;
            return false;
        }

        const auto standardREXPPQ = 15360.0;
        const auto beats = (double) info.ppqLength / standardREXPPQ;
        const auto bps = (double) info.tempo / 60000.0;

        usesFloatingPointData = safeUsesFloatingPointData.load();
        lengthInSamples = (int64) ((beats / bps) * info.sampleRate);
        sampleRate = (double) std::max (0, info.sampleRate);
        bitsPerSample = (uint32) std::max (0, info.bitDepth);
        numChannels = (uint32) std::max (0, info.numChannels);

        safeNumChannels = (int) numChannels;

        if (lengthInSamples == 0
            || sampleRate == 0.0
            || bitsPerSample == 0
            || numChannels == 0)
        {
            jassertfalse;
            return false;
        }

        buffer.setSize (info.numChannels, (int) lengthInSamples, false, true, true);
        buffer.clear();

        StringArray beatPoints;

        for (int j = 0; j < info.numSlices; ++j)
        {
            REXSliceInfo sliceInfo;
            if (rs.getSliceInfo == nullptr || ! checkRexError (rs.getSliceInfo (handle->get(), j, sizeof (sliceInfo), &sliceInfo)))
                return false;

            AudioBuffer<float> sliceData (buffer.getNumChannels(), sliceInfo.sampleLength);
            sliceData.clear();

            if (rs.renderSlice == nullptr)
                return false;

            auto chans = (float**) sliceData.getArrayOfWritePointers();
            if (! checkRexError (rs.renderSlice (handle->get(), j, sliceInfo.sampleLength, chans)))
                return false;

            const auto offset = (int64) (((double) sliceInfo.ppqPos / standardREXPPQ) / ((double) info.tempo / 60000.0) * (double) info.sampleRate);
            const auto numSamples = (int) std::min (lengthInSamples - offset, (int64) sliceInfo.sampleLength);

            if (numSamples > 0)
                for (int i = 0; i < sliceData.getNumChannels(); ++i)
                    buffer.addFrom (i, (int) offset, sliceData, i, 0, numSamples);

            beatPoints.add (String (offset));
        }

        metadataValues.set (REXAudioFormat::rexNumerator,   String (info.timeSignatureNumerator));
        metadataValues.set (REXAudioFormat::rexDenominator, String (info.timeSignatureDenominator));
        metadataValues.set (REXAudioFormat::rexTempo,       String ((double) info.tempo / 1000.0));

        if (beatPoints.size() > 0)
            metadataValues.set (REXAudioFormat::rexBeatPoints, beatPoints.joinIntoString (";"));

        REXCreatorInfo creatorInfo;
        if (rs.getCreatorInfo != nullptr
            && ! checkRexError (rs.getCreatorInfo (handle->get(), sizeof (creatorInfo), &creatorInfo)))
        {
            metadataValues.set (REXAudioFormat::rexName, String (creatorInfo.name, 255).trim());
            metadataValues.set (REXAudioFormat::rexCopyright, String (creatorInfo.copyright, 255).trim());
            metadataValues.set (REXAudioFormat::rexEmail, String (creatorInfo.email, 255).trim());
            metadataValues.set (REXAudioFormat::rexURL, String (creatorInfo.url, 255).trim());
            metadataValues.set (REXAudioFormat::rexAdditionalText, String (creatorInfo.freeText, 255).trim());
        }

        return true;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Reader)
};

//==============================================================================
REXAudioFormat::REXAudioFormat (const File& rexLibrary) :
    AudioFormat (TRANS ("REX2 file"), ".rcy .rex .rx2"),
    rexLibraryDLL (rexLibrary)
{
}

String REXAudioFormat::getErrorMessage() const
{
    return isREXSystemLoaded()
            ? rexSystem->getErrorMessage()
            : String();
}

bool REXAudioFormat::isREXSystemLoaded() const
{
    return rexSystem != nullptr && rexSystem->isOk();
}

void REXAudioFormat::reloadREXSystemIfNeeded()
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

    if (! isREXSystemLoaded())
    {
        const auto e = rexSystem != nullptr
                        ? rexSystem->getLastErrorCode()
                        : REXError::dllNotLoaded;

        switch (e)
        {
            case REXError::notEnoughMemoryForDLL:
            case REXError::unableToLoadDLL:
            case REXError::dllTooOld:
            case REXError::dllNotFound:
            case REXError::outOfMemory:
            case REXError::fileCorrupt:
            case REXError::dllNotLoaded:
                if (rexLibraryDLL.hasFileExtension (rexNativeDLLFileExtension))
                    rexSystem.reset (new REXSystem (rexLibraryDLL));
            break;

            default:
            break;
        };
    }
}

AudioFormatReader* REXAudioFormat::createReaderFor (InputStream* sourceStream, const bool deleteStreamIfOpeningFails)
{
    if (auto* fis = dynamic_cast<FileInputStream*> (sourceStream))
    {
        if (! canHandleFile (fis->getFile()))
        {
            if (deleteStreamIfOpeningFails)
                deleteAndZero (sourceStream);

            return nullptr;
        }
    }
    else
    {
        return nullptr; // REX isn't a codec with headers, it's a system reliant on file extensions!
    }

    reloadREXSystemIfNeeded();

    if (isREXSystemLoaded())
    {
        auto reader = std::make_unique<Reader> (*rexSystem, sourceStream, getFormatName());

        if (reader->loadedOk)
            return reader.release();

        if (! deleteStreamIfOpeningFails)
            reader->input = nullptr;
    }

    if (deleteStreamIfOpeningFails)
        deleteAndZero (sourceStream);

    return nullptr;
}

#endif // SQUAREPINE_USE_REX_AUDIO_FORMAT
