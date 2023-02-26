/** Use an instance of this to be able to create SquarePine based audio effects and instruments.

    @see juce::AudioPluginFormatManager, juce::KnownPluginList, juce::AudioProcessorGraph
*/
class SquarePineAudioPluginFormat final : public AudioPluginFormat
{
public:
    /** Default constructor.

        This won't add any processors AudioProcessorGraph related.
        For that, use the other constructor. 
    */
    SquarePineAudioPluginFormat();

    /** An optional constructor that takes in a juce::AudioProcessorGraph so as to allow
        creating its own I/O processors more easily by facilitating adding them to
        a juce::KnownPluginList.

        The graph instance is required because of the IO processor's
        dependency on the graph itself.
    */
    explicit SquarePineAudioPluginFormat (AudioProcessorGraph&);

    //==============================================================================
    /** */
    const OwnedArray<PluginDescription>& getPluginDescriptions() const noexcept { return descriptions; }

    /** */
    void addEffectPluginDescriptionsTo (KnownPluginList&);
    /** */
    void addInstrumentPluginDescriptionsTo (KnownPluginList&);
    /** */
    void addEffectPluginDescriptionsTo (Array<PluginDescription>&);
    /** */
    void addInstrumentPluginDescriptionsTo (Array<PluginDescription>&);
    /** */
    void createEffectPlugins (OwnedArray<AudioPluginInstance>& results);

    /** */
    [[nodiscard]] PluginDescription getDescriptionFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType) const;
    /** */
    [[nodiscard]] std::unique_ptr<AudioPluginInstance> createInstanceFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType);

    //==============================================================================
    /** @internal */
    String getName() const override { return "SquarePine"; }
    /** @internal */
    bool isTrivialToScan() const override { return true; }
    /** @internal */
    StringArray searchPathsForPlugins (const FileSearchPath&, bool, bool) override;
    /** @internal */
    bool pluginNeedsRescanning (const PluginDescription&) override { return false; }
    /** @internal */
    bool doesPluginStillExist (const PluginDescription&) override { return true; }
    /** @internal */
    bool canScanForPlugins() const override { return true; }
    /** @internal */
    FileSearchPath getDefaultLocationsToSearch() override { return {}; }
    /** @internal */
    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept override { return false; }
    /** @internal */
    void findAllTypesForFile (OwnedArray<PluginDescription>&, const String&) override;
    /** @internal */
    void createPluginInstance (const PluginDescription&, double, int, PluginCreationCallback) override;
    /** @internal */
    bool fileMightContainThisPluginType (const String&) override;
    /** @internal */
    String getNameOfPluginFromIdentifier (const String&) override;

private:
    //==============================================================================
    class CreationHelpers;

    using PluginCreationFunction = std::function<AudioPluginInstance* ()>;
    using PluginCreationMap = std::map<String, PluginCreationFunction>;
    using ConstIterator = PluginCreationMap::const_iterator;

    AudioProcessorGraph* graph = nullptr;
    PluginCreationMap pluginCreationMap;
    OwnedArray<PluginDescription> descriptions;

    //==============================================================================
    void addInternalPluginDescriptions();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineAudioPluginFormat)
};
