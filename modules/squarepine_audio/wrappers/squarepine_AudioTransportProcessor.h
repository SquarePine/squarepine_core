/** A juce::AudioTransportSource object wrapped
    neatly in an InternalProcessor.

    Simply call setSource() to change the source!
*/
class AudioTransportProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    AudioTransportProcessor();

    /** Destructor. */
    ~AudioTransportProcessor() override;

    //==============================================================================
    /** Plays the source, if one was present. */
    void play();

    /** Sets the time to the beginning of the audio source and starts playing. */
    void playFromStart();

    /** Stops the transport from playing, if it was at all. */
    void stop();

    /** Settles whether the transport should loop or not. */
    void setLooping (bool shouldLoop);

    /** Inverses the current loop state. */
    void toggleLooping();

    /** @returns true if the transport is set to loop. */
    [[nodiscard]] bool isLooping() const;

    /** @returns true if the transport is actively playing. */
    [[nodiscard]] bool isPlaying() const;

    /** @returns true if the transport has stopped because
        its input stream ran out of data.
    */
    [[nodiscard]] bool hasStreamFinished() const;

    /** Stops the transport from playing (if it was)
        and deletes the last known audio source.

        If the audio source was owned by the transport,
        it will be deleted.
    */
    void clear();

    //==============================================================================
    /** Changes the current time to that of the specified seconds. */
    void setCurrentTime (double seconds);

    /** Changes the current time to that of the specified samples,
        which transport will use its currently known sample rate to
        convert accordingly to seconds.
    */
    void setCurrentTime (int64 samples);

    /** @returns the length of the audio source in seconds. */
    [[nodiscard]] double getLengthSeconds() const;

    /** @returns the length of the audio source in samples using
        the transport's currently known sample rate to
        convert accordingly from seconds.
    */
    [[nodiscard]] int64 getLengthSamples() const;

    /** @returns the length of the audio source in seconds. */
    [[nodiscard]] double getCurrentTimeSeconds() const;

    /** @returns the length of the audio source in samples using
        the transport's currently known sample rate to
        convert accordingly from seconds.
    */
    [[nodiscard]] int64 getCurrentTimeSamples() const;

    //==============================================================================
    /** Changes the audio source by applying an audio source.

        @param readerSource         Sets a an audio source to be played.
                                    Note that passing in a null audio source
                                    will clear the transport.
        @param transportOwnsSource  Set this to true if you want the audio source
                                    to be owned by the transport.

    */
    void setSource (PositionableAudioSource* source,
                    bool transportOwnsSource,
                    int readAheadBufferSize = 0,
                    TimeSliceThread* readAheadThread = nullptr,
                    double sourceSampleRateToCorrectFor = 0.0,
                    int maxNumChannels = 2);

    /** Changes the audio source by applying a reader source.

        @param readerSource         Sets a reader source to be played.
                                    Note that passing in a null reader source
                                    will clear the transport.
        @param transportOwnsSource  Set this to true if you want the reader source
                                    to be owned by the transport.

    */
    void setSource (AudioFormatReaderSource* readerSource,
                    bool transportOwnsSource,
                    int readAheadBufferSize = 0,
                    TimeSliceThread* readAheadThread = nullptr);

    /** Changes the audio source by applying a reader.

        @param reader   Sets a reader to be played.
                        Note that passing in a null reader
                        will clear the transport.
                        The reader will be owned by the transport.
    */
    void setSource (AudioFormatReader* reader,
                    int readAheadBufferSize = 0,
                    TimeSliceThread* readAheadThread = nullptr);

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Audio Transport"); }
    /** @internal */
    Identifier getIdentifier() const override { return "AudioTransportProcessor"; }
    /** @internal */
    bool isInstrument() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void releaseResources() override;

private:
    //==============================================================================
    /** The ID for the looping parameter. */
    CREATE_INLINE_CLASS_IDENTIFIER (looping)

    AudioParameterBool* loopingParam = nullptr;
    AudioSourceProcessor audioSourceProcessor;
    AudioTransportSource* transport = nullptr;
    OptionalScopedPointer<PositionableAudioSource> source;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioTransportProcessor)
};
