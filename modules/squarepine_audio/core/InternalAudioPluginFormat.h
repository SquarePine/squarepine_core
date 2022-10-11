/** */
class InternalAudioPluginFormat : public AudioPluginFormat
{
public:
    /** */
    explicit InternalAudioPluginFormat (AudioProcessorGraph& graph);

    //==============================================================================
    /** */
    void addPluginDescriptions (KnownPluginList& knownPluginList);
    /** */
    void createEffectPlugins (OwnedArray<AudioPluginInstance>& results);

    /** */
    [[nodiscard]] PluginDescription getDescriptionFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType) const;
    /** */
    [[nodiscard]] std::unique_ptr<AudioPluginInstance> createInstanceFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType);

    //==============================================================================
    /** @internal */
    String getName() const override;
    /** @internal */
    void findAllTypesForFile (OwnedArray<PluginDescription>&, const String&) override;
    /** @internal */
    void createPluginInstance (const PluginDescription&, double initialSampleRate, int initialBufferSize, PluginCreationCallback) override;
    /** @internal */
    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept override;
    /** @internal */
    bool fileMightContainThisPluginType (const String&) override;
    /** @internal */
    String getNameOfPluginFromIdentifier (const String&) override;
    /** @internal */
    bool pluginNeedsRescanning (const PluginDescription&) override;
    /** @internal */
    bool doesPluginStillExist (const PluginDescription&) override;
    /** @internal */
    bool canScanForPlugins() const override;
    /** @internal */
    StringArray searchPathsForPlugins (const FileSearchPath&, bool, bool) override;
    /** @internal */
    FileSearchPath getDefaultLocationsToSearch() override;

private:
    //==============================================================================
    class CreationHelpers;

    using PluginCreationFunction = std::function<AudioPluginInstance* ()>;
    using PluginCreationMap = std::map<String, PluginCreationFunction>;
    using ConstIterator = PluginCreationMap::const_iterator;

    AudioProcessorGraph& graph;
    PluginCreationMap pluginCreationMap;
    OwnedArray<PluginDescription> descriptions;
    int numGraphPlugins = 0;

    //==============================================================================
    void addInternalPluginDescriptions();

    //==============================================================================
    InternalAudioPluginFormat() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalAudioPluginFormat)
};
