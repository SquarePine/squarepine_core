//==============================================================================
namespace
{
    inline void prepareInternal (AudioProcessor& parent, AudioProcessor& proc)
    {
        proc.enableAllBuses();
        proc.setPlayHead (parent.getPlayHead());
        proc.setProcessingPrecision (parent.getProcessingPrecision());
        proc.setRateAndBufferSizeDetails (parent.getSampleRate(), parent.getBlockSize());
        proc.prepareToPlay (parent.getSampleRate(), parent.getBlockSize());
        proc.setNonRealtime (parent.isNonRealtime());
    }
}

//==============================================================================
EffectProcessorChain::EffectProcessorChain (EffectProcessorFactory::Ptr epf) :
    factory (epf)
{
    SQUAREPINE_CRASH_TRACER

    jassert (factory != nullptr);
    effects.ensureStorageAllocated (8);
}

//==============================================================================
template<typename Type>
EffectProcessor::Ptr EffectProcessorChain::insertInternal (int destinationIndex, const Type& valueOrRef, InsertionStyle insertionStyle)
{
    SQUAREPINE_CRASH_TRACER
    const ScopedBypass sb (*this);

    if (factory == nullptr)
    {
        Logger::writeToLog ("EffectProcessorChain: factory not found!");
        jassertfalse;
        return {};
    }

    if (auto pluginInstance = factory->createPlugin (valueOrRef))
    {
        prepareInternal (*this, *pluginInstance);

        const auto description = factory->createPluginDescription (valueOrRef);
        auto effect = make_refptr<EffectProcessor> (std::move (pluginInstance), description);

        auto logMessage = String ("EffectProcessorChain: ACTION effect XYZ (\"PLUG\", \"ID\").")
                            .replace ("XYZ", effect->getName())
                            .replace ("PLUG", effect->getPluginName())
                            .replace ("ID", description.createIdentifierString());

        if (insertionStyle == InsertionStyle::append
            || ! isPositiveAndBelow (destinationIndex, getNumEffects()))
        {
            effects.add (effect);
            logMessage = logMessage.replace ("ACTION", "adding");
        }
        else if (insertionStyle == InsertionStyle::insert)
        {
            effects.insert (destinationIndex, effect);
            logMessage = logMessage.replace ("ACTION", "inserting (index " + String (destinationIndex) + ")");
        }
        else
        {
            effects.set (destinationIndex, effect);
            logMessage = logMessage.replace ("ACTION", "setting (index " + String (destinationIndex) + ")");
        }

        Logger::writeToLog (logMessage);

        while (effectLevels.size() < effects.size())
            if (auto* proc = effectLevels.add (new LevelsProcessor()))
                prepareInternal (*this, *proc);

        updateLatency();
        updateHostDisplay();
        return effect;
    }

    jassertfalse;
    return {};
}

EffectProcessor::Ptr EffectProcessorChain::add (int s)                          { return insertInternal (-1, s, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::add (const String& s)                { return insertInternal (-1, s, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, int s)             { return insertInternal (dest, s); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, const String& s)   { return insertInternal (dest, s); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, int s)            { return insertInternal (dest, s, InsertionStyle::replace); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, const String& s)  { return insertInternal (dest, s, InsertionStyle::replace); }
int EffectProcessorChain::getNumEffects() const                                 { return effects.size(); }

void EffectProcessorChain::move (int pluginIndex, int destinationIndex)
{
    SQUAREPINE_CRASH_TRACER
    const ScopedBypass sb (*this);

    effects.swap (pluginIndex, destinationIndex);

    Logger::writeToLog (String ("EffectProcessorChain: moving index ABC to XYZ.")
                            .replace ("ABC", String (pluginIndex))
                            .replace ("XYZ", String (destinationIndex)));
}

void EffectProcessorChain::swap (int index1, int index2)
{
    SQUAREPINE_CRASH_TRACER
    const ScopedBypass sb (*this);

    effects.swap (index1, index2);

    Logger::writeToLog (String ("EffectProcessorChain: swapping index ABC with XYZ.")
                            .replace ("ABC", String (index1))
                            .replace ("XYZ", String (index2)));
}

bool EffectProcessorChain::remove (int index)
{
    SQUAREPINE_CRASH_TRACER
    const ScopedBypass sb (*this);

    const auto startSize = getNumEffects();
    effects.remove (index);

    if (startSize != getNumEffects())
    {
        Logger::writeToLog (String ("EffectProcessorChain: removed index XYZ.")
                                .replace ("XYZ", String (index)));
        return true;
    }

    return false;
}

bool EffectProcessorChain::contains (EffectProcessor::Ptr effect) const
{
    return effects.contains (effect);
}

bool EffectProcessorChain::clear()
{
    SQUAREPINE_CRASH_TRACER
    const ScopedBypass sb (*this);

    const bool changed = ! effects.isEmpty();
    if (changed)
    {
        effects.clear();
        updateLatency();
    }

    if (changed)
    {
        Logger::writeToLog ("EffectProcessorChain: cleared.");
        updateHostDisplay();
    }

    return changed;
}

//==============================================================================
EffectProcessor::Ptr EffectProcessorChain::getEffectProcessor (int index) const
{
    return effects[index];
}

int EffectProcessorChain::indexOf (EffectProcessor::Ptr effect) const
{
    return effects.indexOf (effect);
}

std::optional<String> EffectProcessorChain::getPluginInstanceName (int index) const
{
    return getEffectProperty<String> (index, [&] (EffectProcessor::Ptr e)
    {
        if (auto p = e->plugin)
            return p->getName();

        return String();
    });
}

std::optional<String> EffectProcessorChain::getEffectName (int index) const
{
    return getEffectProperty<String> (index, [] (EffectProcessor::Ptr e) { return e->getName(); });
}

std::optional<AudioPluginPtr> EffectProcessorChain::getPluginInstance (int index) const
{
    return getEffectProperty<AudioPluginPtr> (index, [&] (EffectProcessor::Ptr e) { return e->plugin; });
}

std::optional<PluginDescription> EffectProcessorChain::getPluginDescription (int index) const
{
    return getEffectProperty<PluginDescription> (index, [&] (EffectProcessor::Ptr e) { return e->description; });
}

std::optional<bool> EffectProcessorChain::isBypassed (int index) const
{
    return getEffectProperty<bool> (index, [&] (EffectProcessor::Ptr e) { return e->isBypassed(); });
}

std::optional<float> EffectProcessorChain::getMixLevel (int index) const
{
    return getEffectProperty<float> (index, [&] (EffectProcessor::Ptr e) { return e->getMixLevel(); });
}

std::optional<juce::Rectangle<int>> EffectProcessorChain::getLastWindowBounds (int index) const
{
    return getEffectProperty<juce::Rectangle<int>> (index, [&] (EffectProcessor::Ptr e) { return e->windowBounds; });
}

std::optional<bool> EffectProcessorChain::isPluginMissing (int index) const
{
    return getEffectProperty<bool> (index, [&] (EffectProcessor::Ptr e) { return e->isMissing(); });
}

bool EffectProcessorChain::loadIfMissing (int index)
{
    SQUAREPINE_CRASH_TRACER

    if (isPositiveAndBelow (index, getNumEffects())
        && isPluginMissing (index))
    {
        if (auto effect = effects[index])
        {
            const InternalProcessor::ScopedBypass sb (*this);

            effect->plugin = factory->createPlugin (effect->description);
            return effect->reloadFromStateIfValid();
        }
    }

    return false;
}

void EffectProcessorChain::getChannelLevels (int index, Array<float>& destData)
{
    SQUAREPINE_CRASH_TRACER

    if (auto* lp = effectLevels[index])
        lp->getChannelLevels (destData);
}

void EffectProcessorChain::getChannelLevels (int index, Array<double>& destData)
{
    SQUAREPINE_CRASH_TRACER

    if (auto* lp = effectLevels[index])
        lp->getChannelLevels (destData);
}

void EffectProcessorChain::setMeteringMode (int index, MeteringMode mm)
{
    SQUAREPINE_CRASH_TRACER

    if (auto* lp = effectLevels[index])
        lp->setMeteringMode (mm);
}

std::optional<MeteringMode> EffectProcessorChain::getMeteringMode (int index) const
{
    SQUAREPINE_CRASH_TRACER

    if (auto* lp = effectLevels[index])
        return lp->getMeteringMode();

    return {};
}

//==============================================================================
bool EffectProcessorChain::setEffectProperty (int index, std::function<void (EffectProcessor::Ptr)> func)
{
    SQUAREPINE_CRASH_TRACER

    jassert (func != nullptr);

    if (auto effect = getEffectProcessor (index))
    {
        func (effect);
        return true;
    }

    return false;
}

bool EffectProcessorChain::setEffectName (int index, const String& name)
{
    return setEffectProperty (index, [name] (EffectProcessor::Ptr e) { e->setName (name); });
}

bool EffectProcessorChain::setBypass (int index, bool bypass)
{
    return setEffectProperty (index, [bypass] (EffectProcessor::Ptr e) { e->setBypassed (bypass); });
}

bool EffectProcessorChain::setMixLevel (int index, float mixLevel)
{
    return setEffectProperty (index, [mixLevel] (EffectProcessor::Ptr e) { e->setMixLevel (mixLevel); });
}

//==============================================================================
int EffectProcessorChain::getNumRequiredChannels() const
{
    SQUAREPINE_CRASH_TRACER

    int newRequiredChannels = 0;

    for (auto effect : effects)
    {
        newRequiredChannels = jmax (newRequiredChannels,
                                    effect->description.numInputChannels,
                                    effect->description.numOutputChannels);

        if (auto plugin = effect->plugin)
        {
            for (const auto b : { true, false })
            {
                const int numBs = plugin->getBusCount (b);

                for (int i = 0; i < numBs; ++i)
                    newRequiredChannels = jmax (newRequiredChannels,
                                                plugin->getChannelCountOfBus (b, i));
            }
        }
    }

    return newRequiredChannels;
}

void EffectProcessorChain::updateLatency()
{
    SQUAREPINE_CRASH_TRACER

    requiredChannels = getNumRequiredChannels();

    int newLatency = 0;

    // NB: This is additive because we're processing in serial!
    for (auto effect : effects)
        if (auto plugin = effect->plugin)
            newLatency += plugin->getLatencySamples();

    setLatencySamples (newLatency);
}

void EffectProcessorChain::prepareToPlay (double sampleRate, int estimatedSamplesPerBlock)
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog (String ("EffectProcessorChain: preparing to play (sr: RATE, block: SIZE).")
                            .replace ("RATE", String (sampleRate))
                            .replace ("SIZE", String (estimatedSamplesPerBlock)));

    setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels(), requiredChannels.load());

    effectLevels.clearQuick (true);

    floatBuffers.prepare (numChans, estimatedSamplesPerBlock);
    doubleBuffers.prepare (numChans, estimatedSamplesPerBlock);

    for (int i = 0; i < effects.size(); ++i)
    {
        if (i >= effectLevels.size())
            if (auto* proc = effectLevels.add (new LevelsProcessor()))
                prepareInternal (*this, *proc);

        if (auto effect = effects.getUnchecked (i))
            if (auto plugin = effect->plugin)
                prepareInternal (*this, *plugin);
    }

    updateLatency();
}

//==============================================================================
template<typename FloatType>
void EffectProcessorChain::processInternal (juce::AudioBuffer<FloatType>& source,
                                            MidiBuffer& midiMessages,
                                            BufferPackage<FloatType>& bufferPackage,
                                            const int numChannels,
                                            const int maxNumChannels,
                                            const int numSamples)
{
    SQUAREPINE_CRASH_TRACER

    bufferPackage.prepare (maxNumChannels, numSamples);

    addFrom (bufferPackage.mixingBuffer, source, numChannels, numSamples);

    // Making this copy in case effects are changed around somehow (CRUD, etc):
    auto plugs = effects;

    for (auto effect : plugs)
    {
        if (effect == nullptr)
            continue;

        // Process the effect:
        bufferPackage.effectBuffer.clear();
        addFrom (bufferPackage.effectBuffer, bufferPackage.mixingBuffer, numChannels, numSamples);

        if (effect->plugin == nullptr || effect->plugin->isSuspended())
        {
            bufferPackage.effectBuffer.clear();
        }
        else
        {
            if (effect->canBeProcessed())
                effect->plugin->processBlock (bufferPackage.effectBuffer, midiMessages);
            else
                effect->plugin->processBlockBypassed (bufferPackage.effectBuffer, midiMessages);
        }

        // Add the effect-saturated samples at the specified mix level:
        const auto mixLevel = static_cast<FloatType> (effect->mixLevel.getNextValue());
        jassert (approximatelyEqual (effect->mixLevel.getTargetValue(), 1.0f));

        bufferPackage.lastBuffer.clear();
        addFrom (bufferPackage.lastBuffer, bufferPackage.effectBuffer, numChannels, numSamples, mixLevel);

        // Add the original samples, at a percentage of the original gain, if the effect level isn't 100%:
        if (mixLevel < static_cast<FloatType> (1))
            addFrom (bufferPackage.lastBuffer, bufferPackage.mixingBuffer, numChannels, numSamples, mixLevel);

        // Copy the result:
        bufferPackage.mixingBuffer.clear();
        addFrom (bufferPackage.mixingBuffer, bufferPackage.lastBuffer, numChannels, numSamples);

        effectLevels.getUnchecked (effects.indexOf (effect))
            ->processBlock (bufferPackage.mixingBuffer, midiMessages);
    }

    source.clear();
    addFrom (source, bufferPackage.mixingBuffer, numChannels, numSamples);
}

template<typename FloatType>
void EffectProcessorChain::process (juce::AudioBuffer<FloatType>& buffer,
                                    MidiBuffer& midiMessages,
                                    BufferPackage<FloatType>& package)
{
    SQUAREPINE_CRASH_TRACER

    const ScopedNoDenormals snd;

    const auto numChannels = jmin (buffer.getNumChannels(), requiredChannels.load());
    const auto numSamples = buffer.getNumSamples();

    if (isBypassed()
        || isSuspended()
        || effects.isEmpty()
        || numChannels <= 0
        || numSamples <= 0)
    {
        // @todo Latency compensation. juce::dsp::DelayLine?
        return;
    }

    const auto maxNumChannels = jmax (buffer.getNumChannels(), requiredChannels.load());

    processInternal (buffer, midiMessages, package,
                     numChannels, maxNumChannels, numSamples);
}

void EffectProcessorChain::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)  { process<float> (buffer, midiMessages, floatBuffers); }
void EffectProcessorChain::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) { process<double> (buffer, midiMessages, doubleBuffers); }

//==============================================================================
double EffectProcessorChain::getTailLengthSeconds() const
{
    SQUAREPINE_CRASH_TRACER

    auto largestTailLength = 0.0;

    for (auto effect : effects)
        if (effect != nullptr && effect->canBeProcessed())
            if (auto plugin = effect->plugin)
                largestTailLength = jmax (largestTailLength, plugin->getTailLengthSeconds());

    return largestTailLength;
}

void EffectProcessorChain::setNonRealtime (bool isNonRealtime) noexcept
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog (String ("EffectProcessorChain: setting to ")
                        + (isNonRealtime ? "offline" : "real-time")
                        + ".");

    for (auto effect : effects)
        if (effect != nullptr)
            if (auto plugin = effect->plugin)
                plugin->setNonRealtime (isNonRealtime);
}

void EffectProcessorChain::releaseResources()           { loopThroughEffectsAndCall<&AudioProcessor::releaseResources>(); }
void EffectProcessorChain::reset()                      { loopThroughEffectsAndCall<&AudioProcessor::reset>(); }
void EffectProcessorChain::numChannelsChanged()         { loopThroughEffectsAndCall<&AudioProcessor::numChannelsChanged>(); }
void EffectProcessorChain::numBusesChanged()            { loopThroughEffectsAndCall<&AudioProcessor::numBusesChanged>(); }
void EffectProcessorChain::processorLayoutsChanged()    { loopThroughEffectsAndCall<&AudioProcessor::processorLayoutsChanged>(); }

//==============================================================================
namespace chainIds
{
    CREATE_INLINE_IDENTIFIER (effects)              // Type: Array (of effect states)
    CREATE_INLINE_IDENTIFIER (bypassed)             // Type: bool
    CREATE_INLINE_IDENTIFIER (name)                 // Type: string
    CREATE_INLINE_IDENTIFIER (mixLevel)             // Type: double
    CREATE_INLINE_IDENTIFIER (meteringMode)         // Type: int
    CREATE_INLINE_IDENTIFIER (windowBounds)         // Type: string, from Rectangle::toString
    CREATE_INLINE_IDENTIFIER (pluginDescription)    // Type: string, Base64
    CREATE_INLINE_IDENTIFIER (state)                // Type: string, Base64
}

void EffectProcessorChain::getStateInformation (MemoryBlock& destData)
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog ("EffectProcessorChain: giving the state information to the host.");

    const ScopedSuspend ss (*this);

    DynamicObject obj;
    obj.setProperty (chainIds::bypassedId, isBypassed());

    Array<var> effectVars;

    for (auto effect : effects)
        effectVars.add (toJSON (effect));

    obj.setProperty (chainIds::effectsId, effectVars);

    const auto jsonString = [&]()
    {
        String r;
        {
            MemoryOutputStream mos (2048);
            obj.writeAsJSON (mos, 4, false, 4);
            r = mos.toString();
        }

        beautifyJSON (r);
        return r;
    }();

    MemoryOutputStream mos (destData, true);
    mos.writeText (jsonString, false, false, NewLine::getDefault());
}

var EffectProcessorChain::toJSON (EffectProcessor::Ptr effect) const
{
    SQUAREPINE_CRASH_TRACER

    if (effect == nullptr)
    {
        jassertfalse;
        return {};
    }

    auto* obj = new DynamicObject();

    obj->setProperty (chainIds::nameId,         effect->getName());
    obj->setProperty (chainIds::bypassedId,     effect->isBypassed());
    obj->setProperty (chainIds::mixLevelId,     effect->getMixLevel());
    obj->setProperty (chainIds::windowBoundsId, effect->windowBounds.toString());

    if (auto meteringMode = getMeteringMode (indexOf (effect)); meteringMode.has_value())
        obj->setProperty (chainIds::meteringModeId, static_cast<int> (meteringMode.value()));

    if (auto xml = effect->description.createXml())
        obj->setProperty (chainIds::pluginDescriptionId, Base64::toBase64 (xml->toString()));

    if (auto plugin = effect->plugin)
    {
        MemoryBlock data;
        plugin->getStateInformation (data);

        if (! data.isEmpty())
            obj->setProperty (chainIds::stateId, Base64::toBase64 (data.getData(), data.getSize()));
    }

    return obj;
}

//==============================================================================
void EffectProcessorChain::setStateInformation (const void* const data, const int sizeInBytes)
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog ("EffectProcessorChain: attempting to set the state...");

    if (! MessageManager::getInstance()->isThisTheMessageThread())
    {
        /** All supported 3rd-party plugin formats (eg: VST2, VST3, AU),
            for various reasons, require being created on the message thread!
            Restructure your code to make it so!
        */
        Logger::writeToLog ("EffectProcessorChain: state setting failed -- wrong thread.");
        jassertfalse;
        return;
    }

    const ScopedSuspend ss (*this);

    releaseResources();
    clear();

    var stateVar;

    {
        MemoryInputStream mis (data, sizeInBytes, false);
        stateVar = JSON::parse (mis);
    }

    const bool shouldBypass = [&]()
    {
        if (stateVar.hasProperty (chainIds::bypassedId))
            return static_cast<bool> (stateVar[chainIds::bypassedId]);

        return false;
    }();

    Logger::writeToLog ("EffectProcessorChain: setting state bypassing to \"" + booleanToString (shouldBypass).toLowerCase() + "\".");
    setBypassed (shouldBypass);

    if (stateVar.hasProperty (chainIds::effectsId))
        if (auto effectsVar = stateVar[chainIds::effectsId].getArray())
            for (const auto& effectState : *effectsVar)
                appendEffectFromJSON (effectState);


    updateLatency();
    updateHostDisplay();
}

bool EffectProcessorChain::appendEffectFromJSON (const var& stateVar)
{
    SQUAREPINE_CRASH_TRACER

    const auto descXmlString = [&]()
    {
        const auto r = stateVar[chainIds::pluginDescriptionId].toString();
        MemoryOutputStream mos;
        return Base64::convertFromBase64 (mos, r)
                ? mos.toString()
                : String();
    }();

    XmlDocument doc (descXmlString);
    if (const auto message = doc.getLastParseError(); message.isNotEmpty())
    {
        Logger::writeToLog ("EffectProcessorChain: error parsing effect description --- " + message);
        jassertfalse;
        return false;
    }

    PluginDescription description;
    auto xml = doc.getDocumentElement();
    if (xml == nullptr || ! description.loadFromXml (*xml))
    {
        Logger::writeToLog ("EffectProcessorChain: error parsing PluginDescription...");
        jassertfalse;
        return false;
    }

    if (auto newEffect = insertInternal (-1, description, InsertionStyle::append))
    {
        // It's fine if this is null because the user's system might simply
        // not have the plugin available. This can happen when sharing projects
        // across systems, the user could have updated the plugin which
        // could have potentially changed the PluginDescription, etc...
        if (auto plugin = newEffect->plugin)
        {
            MemoryBlock data;
            plugin->getStateInformation (data);
            newEffect->setDefaultState (data);
            Logger::writeToLog ("EffectProcessorChain: found default state for effect.");
        }

        newEffect->setName (stateVar[chainIds::nameId].toString());

        if (stateVar.hasProperty (chainIds::mixLevelId))
            newEffect->setMixLevel (static_cast<float> (stateVar[chainIds::mixLevelId]));
        else
            Logger::writeToLog ("EffectProcessorChain: missing mix level property...");

        if (stateVar.hasProperty (chainIds::bypassedId))
            newEffect->setBypassed (static_cast<bool> (stateVar[chainIds::bypassedId]));
        else
            Logger::writeToLog ("EffectProcessorChain: missing bypass property...");

        if (stateVar.hasProperty (chainIds::windowBoundsId))
            newEffect->windowBounds = Rectangle<int>::fromString (stateVar[chainIds::windowBoundsId].toString());
        else
            Logger::writeToLog ("EffectProcessorChain: missing window bounds property...");

        const auto meteringMode = [&]()
        {
            if (stateVar.hasProperty (chainIds::meteringModeId))
            {
                const auto meteringModeInt = static_cast<int> (stateVar[chainIds::meteringModeId]);
                return static_cast<MeteringMode> (meteringModeInt);
            }

            return MeteringMode::peak;
        }();

        setMeteringMode (indexOf (newEffect), meteringMode);

        const auto stateData = [&]()
        {
            MemoryOutputStream mos;
            return Base64::convertFromBase64 (mos, stateVar[chainIds::stateId].toString())
                    ? mos.getMemoryBlock()
                    : MemoryBlock();
        }();

        if (! stateData.isEmpty())
        {
            newEffect->setLastState (stateData);
        }
        else
        {
            Logger::writeToLog ("EffectProcessorChain: error parsing effect state!");
            jassertfalse;
        }

        newEffect->reloadFromStateIfValid();

        return true;
    }

    return false;
}
