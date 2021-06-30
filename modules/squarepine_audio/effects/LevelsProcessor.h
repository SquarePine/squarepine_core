/** Use an instance of this within an audio callback of some kind,
    and call getChannelLevels (on the main thread) to get the
    last known audio levels.
*/
class LevelsProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    LevelsProcessor();

    //==============================================================================
    /** */
    void getChannelLevels (Array<float>& destData);
    /** */
    void getChannelLevels (Array<double>& destData);

    //==============================================================================
    /** The list of possible modes for audio level analysis. */
    enum class Mode
    {
        peak = 0,
        rms,
        midSide
    };

    /** Changes the mode of analysis for the audio levels. */
    void setMode (Mode mode);

    /** @returns the current mode for audio levels analysis. */
    Mode getMode() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Levels Meter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "levelsMeter"; }
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    template<typename FloatType>
    struct ChannelDetails final
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

    //==============================================================================
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
