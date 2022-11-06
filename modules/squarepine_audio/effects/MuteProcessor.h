/** Use this processor to smoothly mute incoming audio and MIDI streams.

    This will perform a fade depending on the direction:
    1. If you were muted and then unmuted, this will fade in to avoid a click.
    2. If you were unmuted and then muted, this will fade out to avoid a click.
*/
class MuteProcessor final : public InternalProcessor
{
public:
    /** Constructor.

        @param startMuted Whether this processor should started muted or not.
                          By default, this is passing audio through as normal.
    */
    MuteProcessor (bool startMuted = false);

    //==============================================================================
    /** Mutes or unmutes the incoming audio and MIDI.

        @param shouldBeMuted Whether this processor should be muted or not.
    */
    void setMuted (bool shouldBeMuted);

    /** @returns true if this mute processor is muted. */
    bool isMuted() const noexcept;

    //==============================================================================
    /** @internal */
    const String getName() const override { return NEEDS_TRANS ("Mute"); }
    /** @internal */
    Identifier getIdentifier() const override { return "mute"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;

private:
    //==============================================================================
    AudioParameterBool* muteParameter = nullptr;
    std::atomic<bool> shouldFadeIn { false }, shouldFadeOut { false };

    //==============================================================================
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, MidiBuffer&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MuteProcessor)
};
