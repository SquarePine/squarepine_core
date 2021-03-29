/** A flexible wrapper around a juce::AudioPluginInstance for use in an EffectProcessorChain.

    When created by an EffectProcessorChain, an instance of this is referencable in an
    assortment of ways so you can easily manage bypassing, mixing, state handling,
    and whatever else for the contained plugin.

    @see EffectProcessorChain
*/
class EffectProcessor final
{
public:
    //==============================================================================
    SQUAREPINE_MAKE_SHAREABLE (EffectProcessor)

    //==============================================================================
    /** Constructs an EffectProcessor based on the provided plugin and its description. */
    EffectProcessor (std::shared_ptr<AudioPluginInstance> plugin,
                     const PluginDescription& description);

    //==============================================================================
    /** @returns true if the contained plugin is null, which is interpreted as likely missing. */
    bool isMissing() const noexcept { return plugin == nullptr; }

    /** @returns true if the plugin is not missing nor bypassed,
        and can generally be run as part of a process chain.
    */
    bool canBeProcessed() const noexcept;

    /** @returns true if the plugin was able to be restored from its last known state. */
    bool reloadFromStateIfValid();

    //==============================================================================
    String name;                                    //<
    std::atomic<bool> isBypassed;                   //<
    LinearSmoothedValue<float> mixLevel;            //< The mix level, which is normalised.
    Point<int> lastUIPosition;                      //<
    std::shared_ptr<AudioPluginInstance> plugin;    //<
    const PluginDescription description;            //<
    MemoryBlock defaultState;                       //<
    String lastKnownBase64State;                    //<

private:
    //==============================================================================
    EffectProcessor() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessor)
};
