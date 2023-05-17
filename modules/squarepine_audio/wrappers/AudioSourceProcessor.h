/** An AudioSource object wrapped nicely in an AudioProcessor.

    Simply call setAudioSource() to change the source!
*/
class AudioSourceProcessor final : public InternalProcessor
{
public:
    /** Constructor. */
    AudioSourceProcessor();

    //==============================================================================
    /** Wrap an AudioSource in this processor

        To remove the audio source, simply pass in nullptr.

        @param source           The audio source to process.
        @param takeOwnership    
    */
    void setAudioSource (AudioSource* source, bool takeOwnership = true);

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Audio Source"); }
    /** @internal */
    Identifier getIdentifier() const override { return "AudioSourceProcessor"; }
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
    OptionalScopedPointer<AudioSource> audioSource;
    AudioSourceChannelInfo info;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSourceProcessor)
};
