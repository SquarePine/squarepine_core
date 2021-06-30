/** Use this processor to smoothly mute incoming audio and MIDI streams. */
class MuteProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    MuteProcessor (bool startMuted = false);

    //==============================================================================
    /** Mute or unmute the incoming audio and MIDI */
    void setMuted (bool shouldBeMuted);

    /** @returns true if this mute processor is muted. */
    bool isMuted() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Mute"); }
    /** @internal */
    Identifier getIdentifier() const override { return "mute"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterBool* muteParameter = nullptr;
    std::atomic<bool> shouldFadeIn { false },
                      shouldFadeOut { false };

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteProcessor)
};
