/**

    @see EffectProcessorChain
*/
class EffectProcessorFactory
{
public:
    /** Constructor

        @param knownPluginList The list of plugins to refer to.
    */
    EffectProcessorFactory (KnownPluginList& knownPluginList);

    /** Destructor. */
    virtual ~EffectProcessorFactory();

    //==============================================================================
    /** */
    [[nodiscard]] PluginDescription createPluginDescription (int index) const;

    /** */
    [[nodiscard]] PluginDescription createPluginDescription (const String& fileOrIdentifier) const;

    /** Provided to help templated function - Just returns the description */
    [[nodiscard]] PluginDescription createPluginDescription (const PluginDescription& description) const;

    //==============================================================================
    /** */
    [[nodiscard]] std::shared_ptr<AudioPluginInstance> createPlugin (int index) const;

    /** */
    [[nodiscard]] std::shared_ptr<AudioPluginInstance> createPlugin (const String& fileOrIdentifier) const;

    /** */
    [[nodiscard]] std::shared_ptr<AudioPluginInstance> createPlugin (const PluginDescription& description) const;

    //==============================================================================
    /** */
    using PluginCreationCallback = std::function<void (std::shared_ptr<AudioPluginInstance>, const String&)>;

    /** */
    void createPluginAsync (int index, PluginCreationCallback callback);

    /** */
    void createPluginAsync (const String& fileOrIdentifier, PluginCreationCallback callback);

    /** */
    void createPluginAsync (const PluginDescription& description, PluginCreationCallback callback);

protected:
    //==============================================================================
    KnownPluginList& knownPluginList;

    //==============================================================================
    /** */
    virtual const AudioPluginFormatManager& getAudioPluginFormatManager() const = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessorFactory)
};
