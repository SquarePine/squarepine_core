//==============================================================================
namespace alac
{
    static const char* const formatName = NEEDS_TRANS ("ALAC file");

    static const char* const types[] =
    {
        "ftyp", "mdat", "moov", "pnot", "udta",
        "uuid", "moof", "free", "skip", "jP2 ",
        "wide", "load", "ctab", "imap", "matt",
        "kmat", "clip", "crgn", "sync", "chap",
        "tmcd", "scpt", "ssrc", "PICT"
    };

    static const char* const metadataTypes[] = { "moov", "udta", "meta", "ilst" };

    enum
    {
        channelAtomSize         = 12,
        maxChannels             = 8,
        maxEscapeHeaderBytes    = 8,
        maxSearches             = 16,
        maxCoefs                = 16,
        defaultFramesPerPacket  = 4096
    };

    enum
    {
        FormatAppleLossless     = 'alac',
        FormatLinearPCM         = 'lpcm',

        /** Lossless Definitions */
        CodecFormat             = FormatAppleLossless,
        Version                 = 0,
        CompatibleVersion       = Version,
        DefaultFrameSize        = 4096,

        /** The AudioChannelLayout atom type is not exposed yet so define it here. */
        AudioChannelLayoutAID   = 'chan'
    };

    enum FormatFlag
    {
        isFloat         = (1 << 0),
        isBigEndian     = (1 << 1),
        isSignedInteger = (1 << 2),
        isPacked        = (1 << 3),
        isAlignedHigh   = (1 << 4) 
    };

    /** These are the Channel Layout Tags used in
        the Channel Layout Info portion of the ALAC magic cookie,
    */
    enum class ChannelLayout
    {
        mono            = (100 << 16) | 1,    // C
        stereo          = (101 << 16) | 2,    // L R
        MPEG_3_0_B      = (113 << 16) | 3,    // C L R
        MPEG_4_0_B      = (116 << 16) | 4,    // C L R Cs
        MPEG_5_0_D      = (120 << 16) | 5,    // C L R Ls Rs
        MPEG_5_1_D      = (124 << 16) | 6,    // C L R Ls Rs LFE
        AAC_6_1         = (142 << 16) | 7,    // C L R Ls Rs Cs LFE
        MPEG_7_1_B      = (127 << 16) | 8     // C Lc Rc L R Ls Rs LFE (doc: IS-13818-7 MPEG2-AAC)
    };

    /** AudioChannelLayout from CoreAudioTypes.h.
        We never need the AudioChannelDescription so we remove it.
    */
    struct ALACAudioChannelLayout final
    {
        uint32_t channelLayoutTag = 0,
                 channelBitmap = 0,
                 numberChannelDescriptions = 0;
    };

    struct AudioFormatDescription final
    {
        double sampleRate = 0.0;
        uint32_t formatID = 0,
                 formatFlags = 0,
                 bytesPerPacket = 0,
                 framesPerPacket = 0,
                 bytesPerFrame = 0,
                 channelsPerFrame = 0,
                 bitsPerChannel = 0,
                 reserved = 0;
    };

    /** @note this struct is wrapped in an 'alac' atom in the sample description extension area
        @note in QT movies, it will be further wrapped in a 'wave' atom surrounded by 'frma' and 'term' atoms
    */
    struct ALACSpecificConfig final
    {
        uint32_t frameLength = 0;
        uint8_t compatibleVersion = 0,
                bitDepth = 0,
                pb = 0,
                mb = 0,
                kb = 0,
                numChannels = 0;
        uint16_t maxRun = 0;
        uint32_t maxFrameBytes = 0,
                 avgBitRate = 0,
                 sampleRate = 0;
    };

    /** */
    struct M4AHeader final
    {
        M4AHeader() = default;

        bool parse (InputStream& stream)
        {
            stream.readInt ();

            return true;
        }

        using FTypeCode = std::array<size_t, 4>;

        int numBytes = 0,
            totalSizeBytes = 0,
            type = 0, // ftyp = 66 74 79 70
            subType = 0,
            subSizeBytes = 0
            ;
    };
}

//==============================================================================
class ALACAudioFormatReader final : public AudioFormatReader
{
public:
    ALACAudioFormatReader (InputStream* in) :
        AudioFormatReader (in, alac::formatName)
    {
    }

    //==============================================================================
    bool readSamples (int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
                      int64 startSampleInFile, int numSamples) override
    {
        clearSamplesBeyondAvailableLength (destSamples, numDestChannels, startOffsetInDestBuffer,
                                           startSampleInFile, numSamples, lengthInSamples);

        if (numSamples <= 0)
            return true;

        if (! input->setPosition (dataChunkStart + startSampleInFile * bytesPerFrame))
            return true;

        while (numSamples > 0)
        {
            tempBuffer.fillWith (0);

            const auto numThisTime = jmin (tempBufSize / bytesPerFrame, numSamples);
            input->readIntoMemoryBlock (tempBuffer, (ssize_t) (numThisTime * bytesPerFrame));

            if (littleEndian)
                copySampleData<AudioData::LittleEndian> (bitsPerSample, usesFloatingPointData,
                                                         destSamples, startOffsetInDestBuffer, numDestChannels,
                                                         tempBuffer.getData(), (int) numChannels, numThisTime);
            else
                copySampleData<AudioData::BigEndian> (bitsPerSample, usesFloatingPointData,
                                                      destSamples, startOffsetInDestBuffer, numDestChannels,
                                                      tempBuffer.getData(), (int) numChannels, numThisTime);

            startOffsetInDestBuffer += numThisTime;
            numSamples -= numThisTime;
        }

        return true;
    }

    template<typename Endianness>
    static void copySampleData (unsigned int numBitsPerSample, bool floatingPointData,
                                int* const* destSamples, int startOffsetInDestBuffer, int numDestChannels,
                                const void* sourceData, int numberOfChannels, int numSamples) noexcept
    {
        switch (numBitsPerSample)
        {
            case 16:    ReadHelper<AudioData::Int32, AudioData::Int16, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples); break;
            case 24:    ReadHelper<AudioData::Int32, AudioData::Int24, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples); break;
            case 32:    if (floatingPointData) ReadHelper<AudioData::Float32, AudioData::Float32, Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples);
                        else                   ReadHelper<AudioData::Int32,   AudioData::Int32,   Endianness>::read (destSamples, startOffsetInDestBuffer, numDestChannels, sourceData, numberOfChannels, numSamples);
                        break;
            default:    jassertfalse; break;
        }
    }

    int bytesPerFrame = 0;
    int64 dataChunkStart = 0;
    bool littleEndian = true;

private:
    const int tempBufSize = 480 * 3 * 4; // (keep this a multiple of 3)
    MemoryBlock tempBuffer { (size_t) tempBufSize };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ALACAudioFormatReader)
};

//==============================================================================
class ALACAudioFormatWriter final : public AudioFormatWriter
{
public:
    ALACAudioFormatWriter (OutputStream* out, double rate,
                           unsigned int numChans, unsigned int bits,
                           const StringPairArray&) :
        AudioFormatWriter (out, TRANS (alac::formatName), rate, numChans, bits)
    {
    }

    ~ALACAudioFormatWriter() override
    {
    }

    //==============================================================================
    bool write (const int** data, int numSamples) override
    {
        jassertquiet (numSamples >= 0);
        jassertquiet (data != nullptr && *data != nullptr); // the input must contain at least one channel!

        return true;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ALACAudioFormatWriter)
};

//==============================================================================
ALACAudioFormat::ALACAudioFormat() :
    AudioFormat (TRANS (alac::formatName), ".m4a .caf")
{
}

Array<int> ALACAudioFormat::getPossibleSampleRates()
{
    return
    {
        8000, 11025, 12000, 16000, 22050, 32000, 44100,
        48000, 88200, 96000, 176400, 192000, 352800, 384000
    };
}

Array<int> ALACAudioFormat::getPossibleBitDepths()
{
    return { 16, 24, 32 };
}

AudioFormatReader* ALACAudioFormat::createReaderFor (InputStream* sourceStream, bool deleteStreamIfOpeningFails)
{
    auto w = std::make_unique<ALACAudioFormatReader> (sourceStream);

    if (w != nullptr && w->sampleRate > 0.0 && w->numChannels > 0)
        return w.release();

    if (! deleteStreamIfOpeningFails)
        w->input = nullptr;

    return nullptr;
}

AudioFormatWriter* ALACAudioFormat::createWriterFor (OutputStream* out,
                                                     double sampleRate,
                                                     unsigned int numberOfChannels,
                                                     int bitsPerSample,
                                                     const StringPairArray& metadataValues,
                                                     int)
{
    if (out != nullptr
        && getPossibleSampleRates().contains ((int) sampleRate)
        && getPossibleBitDepths().contains (bitsPerSample)
        && numberOfChannels > 0
        && isPositiveAndBelow ((int) numberOfChannels, (int) alac::maxChannels))
        return new ALACAudioFormatWriter (out, sampleRate, numberOfChannels,
                                          (unsigned int) bitsPerSample, metadataValues);

    return nullptr;
}
