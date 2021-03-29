/** Use this processor to smoothly mute incoming audio and MIDI streams. */
class MuteProcessor final : public InternalProcessor
{
public:
    /** Constructor */
    MuteProcessor();

    //==============================================================================
    /** Mute or unmute the incoming audio and MIDI */
    void setMuted (bool shouldBeMuted);

    /** @returns true if this mute processor is muted. */
    bool isMuted() const;

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Mute"); }
    /** @internal */
    Identifier getIdentifier() const override { return "muteId"; }
    /** @internal */
    void processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;

private:
    //==============================================================================
    class MuteParameter;
    MuteParameter* muteParameter = nullptr;
    std::atomic<bool> shouldFadeIn { false }, shouldFadeOut { false };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteProcessor)
};
