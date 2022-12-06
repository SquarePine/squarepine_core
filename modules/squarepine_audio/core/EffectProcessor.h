/** A flexible wrapper around a juce::AudioPluginInstance for use in an EffectProcessorChain.

    When created by an EffectProcessorChain, an instance of this is referencable in an
    assortment of ways so you can easily manage bypassing, mixing, state handling,
    and whatever else for the contained plugin.

    @see EffectProcessorChain
*/
class EffectProcessor final : public ReferenceCountedObject,
                              public ValueTree::Listener
{
public:
    //==============================================================================
    using Ptr = ReferenceCountedObjectPtr<EffectProcessor>;

    //==============================================================================
    /** Constructs an EffectProcessor based on the provided plugin and its description.

        Note that the PluginDescription here is separated so as to allow you
        to create an EffectProcessor that is intended to house a valid plugin
        eventually, deferring its creation to the parent EffectProcessorChain
        by performing a reloading of its state.

        In other words, you can have an active but temporarily empty EffectProcessor
        with details to create and reload a plugin instance.

        @see EffectProcessorChain::loadIfMissing
    */
    EffectProcessor (AudioPluginPtr, const PluginDescription&);

    /** */
    ~EffectProcessor() override;

    //==============================================================================
    /** @returns */
    [[nodiscard]] AudioPluginPtr getPlugin() const { return plugin; }

    /** @returns true if the contained plugin is null, which is interpreted as likely missing. */
    [[nodiscard]] bool isMissing() const noexcept { return plugin == nullptr; }

    /** @returns true if the plugin is not missing nor bypassed,
        and can generally be run as part of a process chain.
    */
    [[nodiscard]] bool canBeProcessed() const noexcept;

    //==============================================================================
    /** @returns */
    [[nodiscard]] String getName() const;
    /** */
    void setName (const String&, UndoManager* undoManager = nullptr);
    /** @returns */
    [[nodiscard]] Value getNameValueObject (UndoManager*, bool shouldUpdateSynchronously = false);

    /** @returns */
    [[nodiscard]] float getMixLevel() const;
    /** */
    void setMixLevel (float, UndoManager* undoManager = nullptr);
    /** @returns */
    [[nodiscard]] Value getMixValueObject (UndoManager*, bool shouldUpdateSynchronously = false);

    /** @returns */
    [[nodiscard]] bool isBypassed() const;
    /** */
    void setBypassed (bool, UndoManager* undoManager = nullptr);
    /** @returns */
    [[nodiscard]] Value getBypassValueObject (UndoManager*, bool shouldUpdateSynchronously = false);

    //==============================================================================
    /** @returns true if the plugin was able to be restored from its last known state. */
    bool reloadFromStateIfValid();

    //==============================================================================
    /** @internal */
    void valueTreePropertyChanged (ValueTree&, const Identifier&) override;

private:
    //==============================================================================
    friend class EffectProcessorChain;

    ValueTree state { "state" };

    std::atomic<bool> bypassed { false };           //<
    LinearSmoothedValue<float> mixLevel { 1.0f };   //< The normalised mix level.
    juce::Point<int> lastUIPosition;                //<
    AudioPluginPtr plugin;                          //<
    const PluginDescription description;            //<

    //==============================================================================
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (name)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (mix)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (bypassed)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (defaultState)
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (lastState)

    void setDefaultState (const MemoryBlock&);
    void setLastState (const MemoryBlock&);

    //==============================================================================
    EffectProcessor() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessor)
};
