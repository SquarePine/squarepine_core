/** An AudioSource object wrapped nicely in an AudioProcessor.

    Simply call setAudioSource() to change the source!
*/
class AudioSourceProcessor : public InternalProcessor
{
public:
    /** Constructor */
    AudioSourceProcessor();

    //==============================================================================
    /** Wrap an AudioSource in this processor

        To remove the audio source, simply pass in nullptr.
    */
    void setAudioSource (AudioSource* source, bool takeOwnership = true);

    //==============================================================================
    /** @internal */
    virtual Identifier getIdentifier() const override;
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    bool isInstrument() const override { return true; }

private:
    //==============================================================================
    OptionalScopedPointer<AudioSource> audioSource;
    AudioSourceChannelInfo info;
    juce::AudioBuffer<float> intermittentBuffer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSourceProcessor)
};
