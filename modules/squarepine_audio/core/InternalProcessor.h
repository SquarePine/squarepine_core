/** A base class for all of our internal processors.

    It fills out typically unused methods for you.

    @note Do NOT translate your subclass' name; it used for reference point!
*/
class InternalProcessor : public AudioPluginInstance
{
public:
    /** Constructor */
    InternalProcessor();

    //==============================================================================
    virtual bool isInstrument() const;
    virtual String getVersion() const;
    virtual Identifier getIdentifier() const = 0;

    //==============================================================================
    /** Enable or disable this processor */
    void setBypass (bool shouldBeBypassed);

    /** Find out of this processor is bypassed.

        @note This concept helps preserve connections in a graph,
              and simplify programmatically bypassing any processing.
    */
    bool isBypassed() const noexcept;

    /** An RAII mechanism that automatically bypasses/unbypasses an
        InternalProcessor when it goes out of scope.
    */
    class ScopedBypass final
    {
    public:
        /** */
        ScopedBypass (InternalProcessor& ip);
        /** */
        ~ScopedBypass();

    private:
        InternalProcessor& internalProcessor;
        const bool wasBypassed;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedBypass)
    };

    //==============================================================================
    /** @internal */
    bool acceptsMidi() const override;
    /** @internal */
    bool producesMidi() const override;
    /** @internal */
    const String getName() const override;
    /** @internal */
    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    double getTailLengthSeconds() const override;
    /** @internal */
    bool hasEditor() const override;
    /** @internal */
    AudioProcessorEditor* createEditor() override;
    /** @internal */
    int getNumPrograms() override;
    /** @internal */
    int getCurrentProgram() override;
    /** @internal */
    void setCurrentProgram (int index) override;
    /** @internal */
    const String getProgramName (int index) override;
    /** @internal */
    void changeProgramName (int index, const String& newName) override;
    /** @internal */
    void fillInPluginDescription (PluginDescription& description) const final;
    /** @internal */
    void getStateInformation (MemoryBlock& destData) override;
    /** @internal */
    void setStateInformation (const void* data, int sizeInBytes) override;
    /** @internal */
    AudioProcessorParameter* getBypassParameter() const override;

private:
    //==============================================================================
    class BypassParameter;
    BypassParameter* bypassParameter = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalProcessor)
};

//==============================================================================
/** This processor class serves as a dummy for purposes like testing.
    This can be handy for inheriting from AudioProcessor when you simply want to
    save/load binary data easily, and not really be part of an audio chain.

    As a processor instance, it doesn't actually do anything useful
    processing-wise, but it behaves like a silent AudioProcessor so it can serve
    as a basic stand-in for something better later.
*/
class DummyProcessor : public InternalProcessor
{
public:
    /** Constructor */
    DummyProcessor() = default;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "Dummy"; }
    /** @internal */
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override { }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyProcessor)
};
