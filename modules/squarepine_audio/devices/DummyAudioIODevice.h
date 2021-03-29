/** Use this class for doing things like faking an audio device
    when needing a placeholder driver or rendering to an audio to a file.
*/
class DummyAudioIODevice final : public AudioIODevice,
                                 public Thread
{
public:
    /** Constructor */
    DummyAudioIODevice (bool isInput, int numChannels = 2, double sampleRate = 44100.0, int bufferSize = 1024);

    /** Destructor */
    ~DummyAudioIODevice() override;

    //==============================================================================
    /** A shortcut to the regular AudioIODevice::open().

        @param numOutputChannels  New number of output channels.
        @param sampleRate         New sample rate.
        @param bufferSizeSamples  New size of I/O buffer to use.

        @returns String::empty
    */
    String open (int numOutputChannels,
                 double sampleRate,
                 int bufferSizeSamples);

    //==============================================================================
    /** */
    void setCurrentBufferSizeSamples (int bufferSize);

    /** */
    void setCurrentSampleRate (double sampleRate);

    /** */
    void setNumChannels (int numChans);

    /** @returns the device's current number of output channels (avoids the whole mask thing...).

        If the device isn't actually open, this value doesn't really mean much.
    */
    int getNumChannels();

    //==============================================================================
    /** @internal */
    StringArray getOutputChannelNames() override;
    /** @internal */
    StringArray getInputChannelNames() override;
    /** @internal */
    Array<double> getAvailableSampleRates() override;
    /** @internal */
    Array<int> getAvailableBufferSizes() override;
    /** @internal */
    int getDefaultBufferSize() override;
    /** @internal */
    String open (const BigInteger& inputChannels, const BigInteger& outputChannels,
                 double sampleRate, int bufferSizeSamples) override;
    /** @internal */
    void close() override;
    /** @internal */
    bool isOpen() override;
    /** @internal */
    void start (AudioIODeviceCallback* callback) override;
    /** @internal */
    void stop() override;
    /** @internal */
    bool isPlaying() override;
    /** @internal */
    String getLastError() override;
    /** @internal */
    int getCurrentBufferSizeSamples() override;
    /** @internal */
    double getCurrentSampleRate() override;
    /** @internal */
    int getCurrentBitDepth() override;
    /** @internal */
    BigInteger getActiveOutputChannels() const override;
    /** @internal */
    BigInteger getActiveInputChannels() const override;
    /** @internal */
    int getOutputLatencyInSamples() override;
    /** @internal */
    int getInputLatencyInSamples() override;
    /** @internal */
    void run() override;

private:
    //==============================================================================
    std::atomic<int> numChannels, bufferSize;
    std::atomic<double> sampleRate;
    std::atomic<bool> opened { false }, playing { false };
    CriticalSection callbackLock;
    AudioIODeviceCallback* callback = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyAudioIODevice)
};
