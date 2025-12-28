/** Use this processor to dynamically crush the audio's bits. */
class BitCrusherProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    BitCrusherProcessor();

    //==============================================================================
    /** */
    void setBitDepth (int);

    /** */
    [[nodiscard]] int getBitDepth() const noexcept;

    /** */
    void setDownsampleFactor (int);

    /** */
    [[nodiscard]] int getDownsampleFactor() const noexcept;

    /** */
    void setDrive (float);

    /** */
    [[nodiscard]] float getDrive() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Bit Crusher"); }
    /** @internal */
    Identifier getIdentifier() const override { return "bitCrusher"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

    /** @internal */
    int getNumPrograms() override;
    /** @internal */
    int getCurrentProgram() override;
    /** @internal */
    void setCurrentProgram (int) override;
    /** @internal */
    const String getProgramName (int) override;
    /** @internal */
    void changeProgramName (int, const String&) override {}

private:
    //==============================================================================
    struct Preset final
    {
        Preset() = default;

        Preset (const String& s, float b, float ds, float d) :
            name (s),
            bitDepth (b),
            downsample (ds),
            drive (d)
        {
        }

        Preset (const Preset&) = default;
        Preset (Preset&&) = default;
        ~Preset() = default;
        Preset& operator= (const Preset&) = default;
        Preset& operator= (Preset&&) = default;

        String name;
        float bitDepth = {},
              downsample = {},
              drive = {};
    };

    template<typename FloatType>
    struct ChannelState
    {
        int holdCounter = 0;
        FloatType heldSample = {};
    };

    using PresetContainerType = std::vector<Preset>;
    using PresetContainerSizeType = std::vector<Preset>::size_type;

    int programIndex = 0;
    std::vector<Preset> presets;

    AudioParameterFloat* bitDepthParam = nullptr;
    AudioParameterFloat* downsampleFactorParam = nullptr;
    AudioParameterFloat* driveParam = nullptr;

    Array<ChannelState<float>> floatStates;
    Array<ChannelState<double>> doubleStates;

    //==============================================================================
    void setCurrentProgramDirectly (int);

    template<typename FloatType>
    static FloatType crush (FloatType sample, ChannelState<FloatType>& state,
                            FloatType levels, FloatType makeup,
                            FloatType drive, int dsFactor);

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, Array<ChannelState<FloatType>>& states);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BitCrusherProcessor)
};
