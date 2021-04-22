/** */
class LevelsProcessor final : public InternalProcessor
{
public:
    /** */
    LevelsProcessor();

    //==============================================================================
    /** */
    void getChannelLevels (Array<float>& destData);
    /** */
    void getChannelLevels (Array<double>& destData);

    //==============================================================================
    /** */
    enum class Mode
    {
        peak = 0,
        rms,
        midSide
    };

    /** */
    void setMode (Mode mode);

    /** */
    Mode getMode() const noexcept { return mode.load (std::memory_order_relaxed); }

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "Levels"; }
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Levels"); }
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    template<typename FloatType>
    struct ChannelDetails
    {
        void prepare (int numChannels)
        {
            channels.resize (numChannels);
            channels.clearQuick();

            tempBuffer.resize (numChannels);
            tempBuffer.clearQuick();
        }

        Array<FloatType> channels, tempBuffer;
    };

    std::atomic<Mode> mode { Mode::peak };

    ChannelDetails<float> floatChannelDetails;
    ChannelDetails<double> doubleChannelDetails;

    template<typename FloatType>
    void getChannelLevels (Array<FloatType>& destData, ChannelDetails<FloatType>& details)
    {
        destData.clearQuick();

        const ScopedLock lock (getCallbackLock());
        destData.addArray (details.channels);
    }

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, ChannelDetails<FloatType>& details)
    {
        const auto numChannels = jmin (buffer.getNumChannels(), getTotalNumInputChannels(), getTotalNumOutputChannels());

        details.tempBuffer.clearQuick();

        switch (mode.load (std::memory_order_relaxed))
        {
            case Mode::peak:
                for (int i = 0; i < numChannels; ++i)
                    details.tempBuffer.add (buffer.getMagnitude (i, 0, buffer.getNumSamples()));
            break;

            case Mode::rms:
                for (int i = 0; i < numChannels; ++i)
                    details.tempBuffer.add (buffer.getRMSLevel (i, 0, buffer.getNumSamples()));
            break;

            case Mode::midSide:
                for (int i = 0; i < numChannels; ++i)
                {
                    const auto v = buffer.getMagnitude (i, 0, buffer.getNumSamples());
                    details.tempBuffer.add (square (v));
                }
            break;

            default:
                jassertfalse;
            break;
        };

        const ScopedLock lock (getCallbackLock());
        details.channels.swapWith (details.tempBuffer);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelsProcessor)
};
