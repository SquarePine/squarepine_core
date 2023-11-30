/** Contains an array of effect plugins that are processed in series.

    Unlike juce::AudioProcessorGraph, which can process connections in parallel,
    this boils down the process serially.
    The main problem this class solves is to contain and simplify
    creating various plugin instances for them to be processed in series,
    with the option of controlling the mix level and bypass of each plugin independently.

    The order of the effects in the chain is the direct processing order.

    This will call updateHostDisplay() in the event that any effect is added,
    removed/cleared, or has changed position in the list. This is because
    the latency may change, among other things. Use a juce::AudioProcessorListener
    to be notified of any such changes.

    EffectProcessorFactory is the main class that helps control creation of effect
    based AudioProcessor objects. This is absolutely necessary so as to be able to
    save and recall an EffectProcessorChain's state!

    @warning The bypass in this case is a separate parameter from that of a
             contained plugin's bypass. This is because not every plugin or
             plugin format supports bypassing, resulting in the need of
             a higher level function. On the bright side, the chain and
             the subsequent EffectProcessor instances will help you store
             and restore the bypassing state along with the mix level,
             and the state of the plugin itse;f.

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

    /** @returns the index of the effect or -1 if it wasn't found. */
    [[nodiscard]] int indexOf (EffectProcessor::Ptr) const;

    /** The type of container that manages a list of effect instances. */
    using ContainerType = ReferenceCountedArray<EffectProcessor>;

    /** @returns all of the effects. */
    [[nodiscard]] ContainerType getEffects() const { return effects; }

    //==============================================================================
    /** Add a new effect at the end of the existing array of plugins.

        @param pluginIndex Plugin index within the KnownPluginList.

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr add (int pluginIndex);

    /** Add a new effect at the end of the existing array of plugins.

        @param fileOrIdentifier Plugin file or identifier within the KnownPluginList.

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr add (const String& fileOrIdentifier);

    /** Sets or inserts a new effect with the given plugin index.

        @param destinationIndex Destination in the index of the array of plugins.
        @param pluginIndex      Plugin index within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr insert (int destinationIndex, int pluginIndex);

    /** Sets or inserts a new effect with the given plugin file or identifier.

        @param destinationIndex Destination in the index of the array of plugins.
        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr insert (int destinationIndex, const String& fileOrIdentifier);

    /** Attempts to replace an effect with the given plugin index.

        @param destinationIndex Destination in the index of the array of plugins.
        @param pluginIndex      Plugin index within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr replace (int destinationIndex, int pluginIndex);

    /** Attempts to replace an effect with the given plugin file or identifier.

        @param destinationIndex Destination in the index of the array of plugins.
        @param fileOrIdentifier File or identifier that can be referenced within the KnownPluginList.

        @warning Callers need to remove plugin editors associated with the current index
                 or there will be dangling pointers to the removed plugin!

        @returns a new effect processor. This will be nullptr if the index wasn't found
        or if the plugin failed to load for some reason (eg: debugger is was detected).
    */
    EffectProcessor::Ptr replace (int destinationIndex, const String& fileOrIdentifier);

    //==============================================================================
    /** Move a plugin to a specified index.

        @param sourceIndex      Plugin index within the array of plugins.
        @param destinationIndex Destination in the index of the array of plugins.

        @returns true if the action was successful.
    */
    void move (int sourceIndex, int destinationIndex);

    /** Swaps a pair of effects in the chain.

        If either of the indexes passed in is out-of-range, nothing will happen,
        otherwise the two effects at these positions will be exchanged.
    */
    void swap (int index1, int index2);

    /** Remove an effect at the specific index, or a no-op if it's out of range.

        @param index Index within the array of plugins.

        @returns true if the action was successful.
    */
    bool remove (int index);

    /** @returns true if this chain contains the given effect. */
    [[nodiscard]] bool contains (EffectProcessor::Ptr) const;

    /** Removes all of the effects in the chain. */
    bool clear();

    //==============================================================================
    /** Change the display name of a contained effect.

        @param index    Index within the array of effects.
        @param name     New name.

        @returns true if the action changed anything.
    */
    bool setEffectName (int index, const String& name);

    /** Set whether to bypass or process a contained effect.

        @param index        Index within the array of effects.
        @param shouldBypass Set to true if you would like the effect to be bypassed.

        @returns true if the action changed anything.
    */
    bool setBypass (int index, bool shouldBypass);

    /** Change the mix level of a contained effect.

        @param index    Index within the array of effects.
        @param mixLevel Normalised range; from 0.0f to 1.0f.

        @returns true if the action changed anything.
    */
    bool setMixLevel (int index, float mixLevel);

    //==============================================================================
    /** Obtain the name of a plugin that exists within the array of effects.

        @param index Index within the array of effect plugins.

        @returns the name of the effect, {} otherwise.
    */
    [[nodiscard]] std::optional<String> getEffectName (int index) const;

    /** Obtain the name of a plugin that exists within the array of effect plugins.

        @param index Index within the array of effect plugins.

        @returns the name of the plugin instance, {} otherwise.
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

    /** @returns true if the effect at the specified index is bypassed, {} otherwise.
    */
    [[nodiscard]] std::optional<bool> isBypassed (int index) const;

    /** @returns the mix level of the effect at the specified index (normalised, 0.0f to 1.0f).
        This will return {} if the index is out of range.
    */
    [[nodiscard]] std::optional<float> getMixLevel (int index) const;

    /** @returns the last known effect editor's window bounds, or {} otherwise. */
    [[nodiscard]] std::optional<juce::Rectangle<int>> getLastWindowBounds (int index) const;

    //==============================================================================
    /** Retrieve the float-processed channel levels.

        @param[in] index        The index of the plugin whose channel levels you want to retrieve.
        @param[in/out] destData The measured levels, whose method of measurement relies on the set MeteringMode.

        @see setMeteringMode
    */
    void getChannelLevels (int index, Array<float>& destData);

    /** Retrieve the double-processed channel levels.

        @param[in] index        The index of the plugin whose channel levels you want to retrieve.
        @param[in/out] destData The measured levels, whose method of measurement relies on the set MeteringMode.

        @see setMeteringMode
    */
    void getChannelLevels (int index, Array<double>& destData);

    /** Changes the mode of analysis for the audio levels for the
        particular effect at the provided index.

        @see getChannelLevels
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

        void prepare (int numChans, int numSamps)
        {
            numChannels = std::max (numChannels, numChans);
            numSamples = std::max (numSamples, numSamps);

            for (auto& buff : buffers)
            {
                buff->setSize (numChannels, numSamples, false, false, true);
                buff->clear();
            }
        }

        int numChannels = 0, numSamples = 0;
        Buffer mixingBuffer, effectBuffer, lastBuffer;
        std::array<Buffer*, 3> buffers = { &mixingBuffer, &effectBuffer, &lastBuffer };
    };

    //==============================================================================
    EffectProcessorFactory::Ptr factory;
    std::atomic<int> requiredChannels { 0 };
    BufferPackage<float> floatBuffers;
    BufferPackage<double> doubleBuffers;
    ContainerType effects;
    OwnedArray<LevelsProcessor> effectLevels;

    //==============================================================================
    enum class InsertionStyle
    {
        insert,
        append,
        replace
    };

    void updateLatency();
    [[nodiscard]] int getNumRequiredChannels() const;
    [[nodiscard]] var toJSON (EffectProcessor::Ptr) const;
    bool appendEffectFromJSON (const var&);
    [[nodiscard]] bool setEffectProperty (int index, std::function<void (EffectProcessor::Ptr)>);

    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>&, MidiBuffer&, BufferPackage<FloatType>&);

    template<typename FloatType>
    void processInternal (juce::AudioBuffer<FloatType>& source, MidiBuffer&,
                          BufferPackage<FloatType>&,
                          int sourceNumChannels, int maxNumChannels, int numSamples);

    template<typename Type>
    [[nodiscard]] EffectProcessor::Ptr insertInternal (int destinationIndex, const Type& valueOrRef, InsertionStyle insertionStyle = InsertionStyle::insert);

    template<typename Type>
    std::optional<Type> getEffectProperty (int index, std::function<Type (EffectProcessor::Ptr)> func) const
    {
        SQUAREPINE_CRASH_TRACER

        if (isPositiveAndBelow (index, getNumEffects()))
            if (auto effect = effects[index])
                return { func (effect) };

        return {};
    }

    template<void (AudioProcessor::*function)()>
    void loopThroughEffectsAndCall()
    {
        SQUAREPINE_CRASH_TRACER

        for (auto effect : effects)
            if (effect != nullptr)
                if (auto* plugin = effect->plugin.get())
                    (plugin->*function)();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectProcessorChain)
};
