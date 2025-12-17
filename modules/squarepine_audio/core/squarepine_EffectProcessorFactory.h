/**

    @see EffectProcessorChain
*/
class EffectProcessorFactory
{
public:
    //==============================================================================
    /** Shorthand for creating shared EffectProcessorFactory instances. */
    using Ptr = std::shared_ptr<EffectProcessorFactory>;

    //==============================================================================
    /** Constructor

        @param knownPluginList The list of plugins to refer to.
    */
    EffectProcessorFactory (AudioPluginFormatManager&, KnownPluginList&);

    /** Destructor. */
    virtual ~EffectProcessorFactory();

    //==============================================================================
    /** */
    [[nodiscard]] const AudioPluginFormatManager& getAudioPluginFormatManager() const noexcept { return audioPluginFormatManager; }
    /** */
    [[nodiscard]] AudioPluginFormatManager& getAudioPluginFormatManager() noexcept { return audioPluginFormatManager; }

    /** */
    [[nodiscard]] const KnownPluginList& getKnownPluginList() const noexcept { return knownPluginList; }
    /** */
    [[nodiscard]] KnownPluginList& getKnownPluginList() noexcept { return knownPluginList; }

    //==============================================================================
    /** */
    [[nodiscard]] PluginDescription createPluginDescription (int index) const;

    /** */
    [[nodiscard]] PluginDescription createPluginDescription (const String& fileOrIdentifier) const;

    /** Provided to help templated functions - Just returns the description */
    [[nodiscard]] PluginDescription createPluginDescription (const PluginDescription&) const;

    //==============================================================================
    /** */
    [[nodiscard]] AudioPluginPtr createPlugin (int index) const;

    /** */
    [[nodiscard]] AudioPluginPtr createPlugin (const String& fileOrIdentifier) const;

    /** */
    [[nodiscard]] AudioPluginPtr createPlugin (const PluginDescription&) const;

    //==============================================================================
    /** */
    using PluginCreationCallback = std::function<void (AudioPluginPtr, const String&)>;

    /** */
    void createPluginAsync (int index, PluginCreationCallback);

    /** */
    void createPluginAsync (const String& fileOrIdentifier, PluginCreationCallback);

    /** */
    void createPluginAsync (const PluginDescription&, PluginCreationCallback);

protected:
    //==============================================================================
    AudioPluginFormatManager& audioPluginFormatManager;
    KnownPluginList& knownPluginList;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessorFactory)
};
