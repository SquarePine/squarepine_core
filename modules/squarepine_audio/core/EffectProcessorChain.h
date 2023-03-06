/** Contains an array of effect plugins that connect to each other in series.

    This will call updateHostDisplayin the event that any effect is added,
    removed/cleared, or has changed position in the list. This is because
    the latency may change, among other things.
    Use a AudioProcessorListener to be notified of any such changes.

    @note EffectProcessorFactory is the main class that helps control creation of effect
          based AudioProcessor objects. This is absolutely necessary so as to be able to
          save and recall an EffectProcessorChain's state!

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
    /** Constructs an empty effect processor chain.

        An EffectProcessorFactory must be provided so as to be able to
        create and add/insert new effects.

        @param factory An existing factory used to create instances of effects internally.

        @see EffectProcessorFactory
    */
    EffectProcessorChain (EffectProcessorFactory::Ptr);

    //==============================================================================
    /** @returns the factory that this chain uses to create plugin instances. */
    [[nodiscard]] EffectProcessorFactory::Ptr getFactory() const { return factory; }

    /** @returns the current number of effect processors in this chain. */
    [[nodiscard]] int getNumEffects() const;

    /** @returns the effect at the provided index, or nullptr if it was out of range. */
    [[nodiscard]] EffectProcessor::Ptr getEffectProcessor (int index) const;

    //==============================================================================
    /** Add a new effect at the end of the existing array of plugins.

        @param pluginIndex Plugin index within the KnownPluginList.

        @returns a new effect processor or nullptr if the index wasn't found.
    */
    EffectProcessor::Ptr add (int pluginIndex);

    /** Add a new effect at the end of the existing array of plugins.

        @param fileOrIdentifier Plugin file or identifier within the KnownPluginList.

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr add (const String& fileOrIdentifier);

    /** Sets or inserts a new effect with the given plugin index.

        @param destinationIndex Destination in the index of the array of plugins.
        @param pluginIndex      Plugin index within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the index wasn't found.
    */
    EffectProcessor::Ptr insert (int destinationIndex, int pluginIndex);

    /** Sets or inserts a new effect with the given plugin file or identifier.

        @param destinationIndex Destination in the index of the array of plugins.
        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the identifier wasn't found.
    */
    EffectProcessor::Ptr insert (int destinationIndex, const String& fileOrIdentifier);

    /** Attempts to replace an effect with the given plugin index.

        @param destinationIndex Destination in the index of the array of plugins.
        @param pluginIndex      Plugin index within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor or nullptr if the index wasn't found.
    */
    EffectProcessor::Ptr replace (int destinationIndex, int pluginIndex);

    /** Attempts to replace an effect with the given plugin file or identifier.

        @param destinationIndex Destination in the index of the array of plugins.
        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor, or nullptr if the identifier wasn't found
                 or the index was out of range.
    */
    EffectProcessor::Ptr replace (int destinationIndex, const String& fileOrIdentifier);

    //==============================================================================
    /** Move a plugin to a specified index.

        @param sourceIndex      Plugin index within the array of plugins.
        @param destinationIndex Destination in the index of the array of plugins.

        @returns true if the action was successful.
    */
    void move (int sourceIndex, int destinationIndex);

    /** Remove an effect at the specific index, or a no-op if it's out of range.

        @param index Index within the array of plugins.

        @returns true if the action was successful.
    */
    bool remove (int index);

    /** Removes all of the effects in the chain. */
    bool clear();

    //==============================================================================
    /** Change the display name of a contained effect.

        @param index    Index within the array of plugins
        @param name     New name.

        @returns true if the action changed anything.
    */
    bool setEffectName (int index, const String& name);

    /** Set whether to bypass or process a contained effect.

        @param index        Index within the array of plugins.
        @param shouldBypass Set to true if you would like the plugin to be bypassed.

        @returns true if the action changed anything.
    */
    bool setBypass (int index, bool shouldBypass);

    /** Change the mix level of a contained effect.

        @param index    Index within the array of plugins.
        @param mixLevel Normalised range; from 0.0f to 1.0f.

        @returns true if the action changed anything.
    */
    bool setMixLevel (int index, float mixLevel);

    //==============================================================================
    /** Obtain the name of a plugin that exists within the array of effect plugins

        @param index Index within the array of effect plugins.

        @returns the name of the effect or {} if the index is out of range.
    */
    [[nodiscard]] std::optional<String> getEffectName (int index) const;

    /** Obtain the name of a plugin that exists within the array of effect plugins.

        @param index Index within the array of effect plugins.

        @returns the name of the plugin instance or {} if the index is out of range.
    */
    [[nodiscard]] std::optional<String> getPluginInstanceName (int index) const;

    /** Obtain the plugin instance of a contained effect.

        This isn't very safe to use, unless you lock using the processor's callback lock
        around the pointer instance to get anything out of it or change its properties.
        But doing that should generate really ugly noise because it will block the audio thread!

        @param index Index of the desired plugin.

        @returns the plugin instance if the index was valid, {} otherwise.
    */
    [[nodiscard]] std::optional<AudioPluginPtr> getPluginInstance (int index) const;

    /** Obtain the plugin description of a contained effect.

        @param index Index of the desired plugin.

        @returns the PluginDescription if the index was valid, {} otherwise.
    */
    [[nodiscard]] std::optional<PluginDescription> getPluginDescription (int index) const;

    /** @returns true if the effect at the specified index is bypassed.
        This will return {} if the index is out of range.
    */
    [[nodiscard]] std::optional<bool> isBypassed (int index) const;

    /** @returns the mix level of the effect at the specified index (normalised, 0.0f to 1.0f).
                 This will return {} if the index is out of range.
    */
    [[nodiscard]] std::optional<float> getMixLevel (int index) const;

    /** @returns the last known top-left position of an effect's editor.
                 This will return {} if the index is out of range.
    */
    [[nodiscard]] std::optional<juce::Point<int>> getLastUIPosition (int index) const;

    //==============================================================================
    /** */
    void getChannelLevels (int index, Array<float>& destData);
    /** */
    void getChannelLevels (int index, Array<double>& destData);

    /** Changes the mode of analysis for the audio levels for the
        particular effect at the provided index.
    */
    void setMeteringMode (int index, MeteringMode);

    /** @returns the current mode for audio levels analysis. */
    [[nodiscard]] std::optional<MeteringMode> getMeteringMode (int index) const;

    //==============================================================================
    /** @returns true if the effect's plugin is missing.

        In the event that you want to try reloading the plugin,
        call loadIfMissing();

        @see loadIfMissing
    */
    [[nodiscard]] std::optional<bool> isPluginMissing (int index) const;

    /** Attempt loading an effect's plugin instance if it is known to be missing.

        @note This will bypass the entire effect chain until the action of
              attempting to load a plugin is complete!
    */
    bool loadIfMissing (int index);

    //==============================================================================
    using InternalProcessor::isBypassed;

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
    void setNonRealtime (bool) noexcept override;
    /** @internal */
    [[nodiscard]] bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    [[nodiscard]] double getTailLengthSeconds() const override;
    /** @internal */
    [[nodiscard]] Identifier getIdentifier() const override { return "EffectProcessorChain"; }
    /** @internal */
    [[nodiscard]] const String getName() const override { return NEEDS_TRANS ("Effect Processor Chain"); }
    /** @internal */
    [[nodiscard]] bool acceptsMidi() const override { return true; }
    /** @internal */
    [[nodiscard]] bool producesMidi() const override { return false; }
    /** @internal */
    void getStateInformation (MemoryBlock&) override;
    /** @internal */
    void setStateInformation (const void*, int) override;

private:
    //==============================================================================
    template<typename FloatType>
    struct BufferPackage final
    {
        using Buffer = juce::AudioBuffer<FloatType>;

        void clear() noexcept
        {
            for (auto& buff : buffers)
                buff->clear();
        }

        void prepare (int numChannels, int numSamples)
        {
            for (auto& buff : buffers)
            {
                buff->setSize (numChannels, numSamples, false, true, true);
                buff->clear();
            }
        }

        Buffer mixingBuffer, effectBuffer, lastBuffer;
        std::array<Buffer*, 3> buffers = { &mixingBuffer, &effectBuffer, &lastBuffer };
    };

    //==============================================================================
    EffectProcessorFactory::Ptr factory;

    using ContainerType = ReferenceCountedArray<EffectProcessor>;
    ContainerType plugins;

    std::atomic<int> requiredChannels { 0 };

    BufferPackage<float> floatBuffers;
    BufferPackage<double> doubleBuffers;

    OwnedArray<LevelsProcessor> effectLevels;

    //==============================================================================
    enum class InsertionStyle
    {
        insert,
        append,
        replace
    };

    [[nodiscard]] bool isWholeChainBypassed() const;
    void updateLatency();
    [[nodiscard]] int getNumRequiredChannels() const;
    [[nodiscard]] XmlElement* createElementForEffect (EffectProcessor::Ptr effect);
    [[nodiscard]] EffectProcessor::Ptr createEffectProcessorFromXML (XmlElement* state);
    [[nodiscard]] bool setEffectProperty (int index, std::function<void (EffectProcessor::Ptr)> func);

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, MidiBuffer&, BufferPackage<FloatType>&);

    template<typename FloatType>
    void processInternal (juce::AudioBuffer<FloatType>& source, MidiBuffer& midiMessages,
                          BufferPackage<FloatType>& bufferPackage, int numChannels, int numSamples);

    template<typename Type>
    [[nodiscard]] EffectProcessor::Ptr insertInternal (int destinationIndex, const Type& valueOrRef, InsertionStyle insertionStyle = InsertionStyle::insert);

    template<typename Type>
    std::optional<Type> getEffectProperty (int index, std::function<Type (EffectProcessor::Ptr)> func) const
    {
        if (isPositiveAndBelow (index, getNumEffects()))
            if (auto effect = plugins[index])
                return { func (effect) };

        return {};
    }

    template<void (AudioProcessor::*function)()>
    void loopThroughEffectsAndCall()
    {
        for (auto effect : plugins)
            if (effect != nullptr)
                if (auto* plugin = effect->plugin.get())
                    (plugin->*function)();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessorChain)
};
