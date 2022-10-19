/** */
class ResamplingAudioFormatReader final : public AudioFormatReader
{
public:
    /** Creates a reader without any sample rate conversion, audio could potentially be played at the wrong rate.

        Remember to call prepare when the desired output sample rate is known.
    */
    ResamplingAudioFormatReader (std::shared_ptr<AudioFormatReader> formatReader,
                                 std::shared_ptr<TimeSliceThread> timeSliceThread = {});

    /** */
    ResamplingAudioFormatReader (std::shared_ptr<AudioFormatReader> formatReader,
                                 int expectedReadBlockSize, double outputSampleRate,
                                 std::shared_ptr<TimeSliceThread> timeSliceThread = {});

    /** */
    ~ResamplingAudioFormatReader() override;

    //==============================================================================
    /** */
    void prepare (double outputRate, int expectedReadBlockSize);

    /** */
    double getConversionRatio() const noexcept { return sourceRatio; }

    //==============================================================================
    /** @internal */
    bool readSamples (int**, int, int, int64, int) override;

    //==============================================================================
    /** The original sample rate of the stream. */
    double originalSampleRate = 0.0;

private:
    //==============================================================================
    juce::AudioBuffer<float> outputBuffer;

    std::shared_ptr<AudioFormatReader> reader;
    std::shared_ptr<TimeSliceThread> timeSliceThread;

    std::unique_ptr<AudioFormatReaderSource> rSource;
    std::unique_ptr<AudioTransportSource> source;

    double sourceRatio = 0.0;
    int sourceBlockSize = 0;

    //==============================================================================
    void writeOutputBuffers (int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer, int numSamples);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResamplingAudioFormatReader)
};
