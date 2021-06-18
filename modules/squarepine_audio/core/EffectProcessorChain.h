/** Contains an array of effect plugins that connect to each other in series.

    @see EffectProcessor, EffectProcessorFactory
*/
class EffectProcessorChain final : public InternalProcessor
{
public:
    //==============================================================================
    SQUAREPINE_MAKE_SHAREABLE (EffectProcessorChain)

    //==============================================================================
    /** */
    CREATE_INLINE_CLASS_IDENTIFIER (effectProcessorChain)

    //==============================================================================
    /** Constructor

        @param factory An existing factory used to create instances of effects internally.
    */
    EffectProcessorChain (std::shared_ptr<EffectProcessorFactory> factory);

    //==============================================================================
    /** @returns the current number of effect processors in this chain. */
    int getNumEffects() const;

    /** @returns the effect at the provided index, or nullptr if it was out of range. */
    EffectProcessor::Ptr getEffectProcessor (int index) const;

    //==============================================================================
    /** Add a new effect at the end of the existing array of plugins.

        @param pluginIndex Plugin index within the KnownPluginList

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr appendNewEffect (int pluginIndex);

    /** Add a new effect at the end of the existing array of plugins.

        @param fileOrIdentifier Plugin identifier within the KnownPluginList

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr appendNewEffect (const String& fileOrIdentifier);

    //==============================================================================
    /** Sets or inserts a new effect with the given plugin index.

        @param pluginIndex      Plugin index within the KnownPluginList
        @param destinationIndex Destination in the index of the array of plugins

        @warning Callers need to remove plugin editors associated with the current slot
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr insertNewEffect (int pluginIndex, int destinationIndex);

    /** Sets or inserts a new effect with the given plugin file or identifier.

        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList
        @param destinationIndex Destination in the index of the array of plugins

        @warning Callers need to remove plugin editors associated with the current slot
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr insertNewEffect (const String& fileOrIdentifier, int destinationIndex);

    //==============================================================================
    /** Attempts replacing an effect with the given plugin index.

        @param pluginIndex      Plugin index within the KnownPluginList
        @param destinationIndex Destination in the index of the array of plugins

        @warning Callers need to remove plugin editors associated with the current slot
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr replaceEffect (int pluginIndex, int destinationIndex);

    /** Attempts replacing an effect with the given plugin file or identifier.

        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList
        @param destinationIndex Destination in the index of the array of plugins

        @warning Callers need to remove plugin editors associated with the current slot
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr replaceEffect (const String& fileOrIdentifier, int destinationIndex);

    //==============================================================================
    /** Enumeration that automates and simplifies reordering a plugin in a chain of plugins. */
    enum class PluginPositionPreset
    {
        shiftToFirst,
        shiftToPrevious,
        shiftToNext,
        shiftToLast
    };

    /** Move a plugin to an index based on a preset.

        @param pluginIndex          Plugin index within the array of plugins
        @param destinationPosition  Destination position based on the available presets

        @returns true if the move was successful.
    */
    bool moveEffect (int pluginIndex, PluginPositionPreset destinationPosition);

    /** Move a plugin to a specified index.

        @param pluginIndex      Plugin index within the array of plugins
        @param destinationIndex Destination in the index of the array of plugins

        @returns true if the move was successful.
    */
    bool moveEffect (int pluginIndex, int destinationIndex);

    //==============================================================================
    /** Remove an effect at the specific index, or a no-op if it's out of range.

        @param index Index within the array of plugins.

        @returns true if the removing was successful.
    */
    bool removeEffect (int index);

    /** Removes all of the effects in the chain.

        @returns true if anything changed.
    */
    bool clear();

    //==============================================================================
    /** Change the display name of a contained effect.

        @param index    Index within the array of plugins
        @param name     New name.

        @returns true if anything changed.
    */
    bool setEffectName (int index, const String& name);

    /** Bypass or process a contained effect

        @param index        Index within the array of plugins.
        @param shouldBypass Set to true if you would like the plugin to be bypassed.

        @returns true if anything changed.
    */
    bool setBypass (int index, bool shouldBypass);

    /** Change the mix level of a contained effect.

        @param index    Index within the array of plugins.
        @param mixLevel Normalised range; from 0.0f to 1.0f.

        @returns true if anything changed.
    */
    bool setMixLevel (int index, float mixLevel);

    //==============================================================================
    /** Obtain the name of a plugin that exists within the array of effect plugins

        @param index Index within the array of effect plugins.

        @returns Name of effect, or String::empty if the index is out of range.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (String) getEffectName (int index) const;

    /** Obtain the name of a plugin that exists within the array of effect plugins.

        @param index Index within the array of effect plugins.

        @returns Name of plugin instance, or String::empty if the index is out of range.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (String) getPluginInstanceName (int index) const;

    /** Obtain the plugin instance of a contained effect.

        This isn't very safe to use, unless you lock using the processor's callback lock
        around the pointer instance to get anything out of it or change its properties.
        But doing that should generate really ugly noise because it will block the audio thread!

        @param index Index of the desired plugin.

        @returns Plugin instance if the index was valid, nullptr otherwise.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (std::shared_ptr<AudioPluginInstance>) getPluginInstance (int index) const;

    /** Obtain the plugin description of a contained effect.

        @param index Index of the desired plugin.

        @returns Proper PluginDescription if the index was valid, PluginDescription() otherwise.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (PluginDescription) getPluginDescription (int index) const;

    /** @returns true if the effect at the specified index is bypassed.
        This will return false if the index is out of range.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (bool) isBypassed (int index) const;

    /** @returns the mix level of the effect at the specified index (normalised, 0.0f to 1.0f).
                 This will return 0.0f if the index is out of range.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (float) getMixLevel (int index) const;

    /** @returns the last known top-left position of an effect's editor.
                 This will return (0, 0) if the index is out of range.
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (juce::Point<int>) getLastUIPosition (int index) const;

    //==============================================================================
    /** @returns true if the effect's plugin is missing.

        In the event that you want to try reloading the plugin,
        call loadIfMissing();

        @see loadIfMissing
    */
    SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (bool) isPluginMissing (int index) const;

    /** Attempt loading an effect's plugin instance if it is known to be missing.

        @note This will bypass the entire effect chain until the action of
              attempting to load a plugin is complete!
    */
    bool loadIfMissing (int index);

    //==============================================================================
    /** @internal */
    void reset() override;
    /** @internal */
    void numChannelsChanged() override;
    /** @internal */
    void numBusesChanged() override;
    /** @internal */
    void processorLayoutsChanged() override;
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void releaseResources() override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    double getTailLengthSeconds() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    const String getName() const override;
    /** @internal */
    bool acceptsMidi() const override;
    /** @internal */
    bool producesMidi() const override;
    /** @internal */
    void getStateInformation (MemoryBlock&) override;
    /** @internal */
    void setStateInformation (const void*, int) override;

private:
    //==============================================================================
    template<typename FloatType>
    struct BufferPackage final
    {
        BufferPackage() noexcept = default;

        void update (int numChannels, int numSamples)
        {
            mixingBuffer.setSize (numChannels, numSamples, false, true, true);
            effectBuffer.setSize (numChannels, numSamples, false, true, true);
            lastBuffer.setSize (numChannels, numSamples, false, true, true);
        }

        void clear() noexcept
        {
            mixingBuffer.clear();
            effectBuffer.clear();
            lastBuffer.clear();
        }

        juce::AudioBuffer<FloatType> mixingBuffer, effectBuffer, lastBuffer;

    private:
        JUCE_DECLARE_NON_COPYABLE (BufferPackage)
    };

    //==============================================================================
    std::shared_ptr<EffectProcessorFactory> factory;
    std::vector<EffectProcessor::Ptr> plugins;

    std::atomic<int> requiredChannels { 0 };

    BufferPackage<float> floatBuffers;
    BufferPackage<double> doubleBuffers;

    //==============================================================================
    bool isWholeChainBypassed() const;
    void updateLatency();
    void updateChannelCount();

    XmlElement* createElementForEffect (EffectProcessor::Ptr effect);
    EffectProcessor::Ptr createEffectProcessorFromXML (XmlElement* state);

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, MidiBuffer&, BufferPackage<FloatType>&);

    template<typename FloatType>
    void processInternal (juce::AudioBuffer<FloatType>& source, MidiBuffer& midiMessages,
                          BufferPackage<FloatType>& bufferPackage, int numChannels, int numSamples);

    enum class InsertionStyle
    {
        insert,
        append,
        replace
    };

    template<typename Type>
    EffectProcessor::Ptr insertInternal (const Type& valueOrRef, int destinationIndex, InsertionStyle insertionStyle = InsertionStyle::insert);

    template<void (AudioProcessor::*function)()>
    void loopThroughEffectsAndCall()
    {

        for (auto EffectProcessor : plugins)
            if (EffectProcessor != nullptr)
                if (auto* plugin = EffectProcessor->plugin.get())
                    (plugin->*function)();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessorChain)
};
