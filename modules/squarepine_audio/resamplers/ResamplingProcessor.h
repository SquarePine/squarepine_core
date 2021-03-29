#if 0

class ResamplingProcessor : public InternalProcessor
{
public:
    /** Constructor */
    template <typename ResamplerType>
    ResamplingProcessor();

    //==============================================================================
    /** Change the resampling ratio.

        A value of 1.0 means no change.
    */
    void setRatio (double newRatio);

    /** Set the ratio via sampling rates.

        @param sourceRate       The source rate of the audio content.
        @param destinationRate  The target rate to correct to.
    */
    void setRatio (double sourceRate, double destinationRate);

    /** @returns The currently set ratio. */
    double getRatio() const noexcept { return ratio.load(); }

    //==============================================================================
    /** Changes the resamplers to use. */
    void setResamplers (Resampler* realtimeResampler, Resampler* offlineResampler);

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Resampler"); }
    /** @internal */
    Identifier getIdentifier() const override { return "Resampler"; }
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    bool acceptsMidi() const override { return false; }
    /** @internal */
    bool producesMidi() const override { return false; }

private:
    //==============================================================================
    std::atomic<double> ratio { 1.0 };
    std::unique_ptr<Resampler> realtime, offline;
    AudioBuffer<float> result;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ResamplingProcessor)
};

#endif //
