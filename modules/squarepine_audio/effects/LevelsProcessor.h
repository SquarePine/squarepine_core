/** The list of possible modes for audio level analysis.

    @see LevelsProcessor, Meter
*/
enum class MeteringMode
{
    peak = 0,
    rms,
    midSide
};

/** Use an instance of this within an audio callback of some kind,
    and call getChannelLevels (on the main thread) to get the
    last known audio levels.

    @see MeteringMode, Meter
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
    /** Changes the mode of analysis for the audio levels. */
    void setMeteringMode (MeteringMode);

    /** @returns the current mode for audio levels analysis. */
    [[nodiscard]] MeteringMode getMeteringMode() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Levels Meter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "levelsMeter"; }
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
    /** Stores the resulting measured levels depending on the MeteringMode. */
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

        template<typename OtherFloatType>
        void process (juce::AudioBuffer<OtherFloatType>& buffer,
                      MeteringMode meteringMode,
                      bool possiblyBypassed)
        {
            tempBuffer.clearQuick();

            if (! possiblyBypassed)
            {
                const auto numChannels = jmin (buffer.getNumChannels(), tempBuffer.size());

                switch (meteringMode)
                {
                    case MeteringMode::peak:
                        for (int i = 0; i < numChannels; ++i)
                            tempBuffer.add ((FloatType) buffer.getMagnitude (i, 0, buffer.getNumSamples()));
                    break;

                    case MeteringMode::rms:
                        for (int i = 0; i < numChannels; ++i)
                            tempBuffer.add ((FloatType) buffer.getRMSLevel (i, 0, buffer.getNumSamples()));
                    break;

                    case MeteringMode::midSide:
                        for (int i = 0; i < numChannels; ++i)
                        {
                            const auto v = buffer.getMagnitude (i, 0, buffer.getNumSamples());
                            tempBuffer.add ((FloatType) square (v));
                        }
                    break;

                    default:
                        jassertfalse;
                    break;
                };
            }

            channels.swapWith (tempBuffer);
        }

        Array<FloatType> channels, tempBuffer;
    };

    class MeteringModeParameter;
    MeteringModeParameter* meteringModeParam = nullptr;

    ChannelDetails<float> floatChannelDetails;
    ChannelDetails<double> doubleChannelDetails;

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelsProcessor)
};
