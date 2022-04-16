/** @returns an array of pointers to a processor's parameters,
    excluding its bypass parameter if set.
*/
inline Array<AudioProcessorParameter*> getAllParametersExcludingBypass (AudioProcessor& processor)
{
    Array<AudioProcessorParameter*> params;

    for (auto* param : processor.getParameters())
        if (param != processor.getBypassParameter())
            params.addIfNotAlreadyThere (param);

    return params;
}

//==============================================================================
/** A base class for basic internal processors; processors which you 
    don't typically expose to the user.

    This class fills out typically unused methods for you,
    and adds a bypass parameter by default.
*/
class InternalProcessor : public AudioPluginInstance
{
public:
    /** Constructs an empty processor with a bypass parameter.

        If you want to configure something more complex,
        create and make use of the AudioProcessorValueTreeState member
        in your constructor to suit your needs.
    */
    InternalProcessor (bool applyDefaultBypassParam = true);

    //==============================================================================
    /** @returns true if this processor represents an instrument.
        This will be used when creating a PluginDescription.
    */
    virtual bool isInstrument() const { return false; }

    /** @returns a string version of this processor.
        This will be used when creating a PluginDescription.
    */
    virtual String getVersion() const { return "1.0"; }

    /** @returns the APVTS ID to use for this processor.
        This will be used when creating a PluginDescription.
    */
    virtual Identifier getIdentifier() const = 0;

    //==============================================================================
    /** @returns the APVTS directly if you want to more easily tie
        logic and UI controls to parameters. This may be null
        seeing that subclasses only optionally need to construct this.

        The InternalProcessor owns this pointer, meaning that
        storing the pointer is done at your own risk!
    */
    JUCE_NODISCARD AudioProcessorValueTreeState* getAPVTS() const { return apvts.get(); }

    /** @returns true if the APVTS instance is non-null. */
    JUCE_NODISCARD bool hasAPVTS() const noexcept { return apvts != nullptr; }

    /** @returns the underlying ValueTree state for the APVTS, assuming one is present.

        Use this and add listeners to it to get any kind of child or property change.
    */
    JUCE_NODISCARD ValueTree getState() const;

    /** @returns a direct Value to a property inside the APVTS.

        If no APVTS is present, or if the Value wasn't found,
        this will return a null/blank Value.
    */
    JUCE_NODISCARD Value getPropertyAsValue (const Identifier&,
                                           UndoManager* undoManager = nullptr,
                                           bool shouldUpdateSynchronously = false);

    /** @returns the value of a named property.

        If no such property has been set, this will return a void variant.

        @see var, setProperty, getPropertyPointer, hasProperty
    */
    JUCE_NODISCARD const var& getProperty (const Identifier&) const;

    /** @returns the value of a named property, or the value of defaultReturnValue if the property doesn't exist.

        @see var, getProperty, getPropertyPointer, setProperty, hasProperty
    */
    JUCE_NODISCARD var getProperty (const Identifier&, const var& defaultReturnValue) const;

    /** @returns a pointer to the value of a named property, or nullptr if the property doesn't exist.

        @see var, getProperty, setProperty, hasProperty
    */
    JUCE_NODISCARD const var* getPropertyPointer (const Identifier&) const;

    /** Changes a named property of the project item.

        The name identifier must not be an empty string.

        If the undoManager parameter is not nullptr, its UndoManager::perform() method will be used,
        so that this change can be undone. Be very careful not to mix undoable and non-undoable changes!

        @see var, getProperty, removeProperty
        @returns a reference to the project item so that you can daisy-chain calls to this method.
    */
    InternalProcessor& setProperty (const Identifier&, const var&, UndoManager* undoManager = nullptr);

    /** @returns true if the project item contains a named property. */
    JUCE_NODISCARD bool hasProperty (const Identifier&) const;

    /** Removes a property from the project item.

        If the undoManager parameter is not null, its UndoManager::perform() method will be used
        so that this change can be undone. Be very careful not to mix undoable and non-undoable changes!
    */
    void removeProperty (const Identifier&, UndoManager* undoManager = nullptr);

    //==============================================================================
    /** Effectively enables or disables this processor. */
    void setBypass (bool shouldBeBypassed);

    /** @returns true if this processor is bypassed.

        @note This concept helps preserve connections in a graph,
              and simplify programmatically bypassing any processing.
    */
    JUCE_NODISCARD bool isBypassed() const noexcept;

    /** The ID for the Bypass parameter. */
    CREATE_INLINE_CLASS_IDENTIFIER (bypass)

    /** An RAII mechanism that automatically bypasses/unbypasses an
        InternalProcessor when it goes out of scope.
    */
    class ScopedBypass final
    {
    public:
        /** Begins bypassing the processor if it wasn't already. */
        ScopedBypass (InternalProcessor& ip);
        /** Restores the last bypassing state. */
        ~ScopedBypass();

    private:
        InternalProcessor& internalProcessor;
        const bool wasBypassed;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScopedBypass)
    };

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS (getIdentifier().toString()); }
    /** @internal */
    bool acceptsMidi() const override { return false; }
    /** @internal */
    bool producesMidi() const override { return false; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void releaseResources() override { }
    /** @internal */
    double getTailLengthSeconds() const override { return 0.0; }
    /** @internal */
    bool hasEditor() const override { return false; }
    /** @internal */
    AudioProcessorEditor* createEditor() override { return nullptr; }
    /** @internal */
    int getNumPrograms() override { return 1; }
    /** @internal */
    int getCurrentProgram() override { return 0; }
    /** @internal */
    void setCurrentProgram (int) override { }
    /** @internal */
    const String getProgramName (int) override { return TRANS ("Default"); }
    /** @internal */
    void changeProgramName (int, const String&) override { }
    /** @internal */
    void fillInPluginDescription (PluginDescription&) const final;
    /** @internal */
    void getStateInformation (MemoryBlock&) override;
    /** @internal */
    void setStateInformation (const void*, int) override;
    /** @internal */
    AudioProcessorParameter* getBypassParameter() const override { return bypassParameter; }

protected:
    //==============================================================================
    std::unique_ptr<AudioProcessorValueTreeState> apvts;

    class BypassParameter;
    AudioParameterBool* bypassParameter = nullptr;

    /** */
    JUCE_NODISCARD std::unique_ptr<AudioParameterBool> createBypassParameter() const;

    /** */
    JUCE_NODISCARD AudioProcessorValueTreeState::ParameterLayout createDefaultParameterLayout (bool addBypassParam = true);

private:
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
    /** Constructor. */
    DummyProcessor() = default;

    //==============================================================================
    /** @internal */
    Identifier getIdentifier() const override { return "Dummy"; }
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override { }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DummyProcessor)
};
