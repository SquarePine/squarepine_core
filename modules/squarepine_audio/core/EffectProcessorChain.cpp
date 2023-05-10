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
        auto prepareInternal = [&] (AudioProcessor& proc)
        {
            proc.setPlayHead (getPlayHead());
            proc.setProcessingPrecision (getProcessingPrecision());
            proc.setRateAndBufferSizeDetails (getSampleRate(), getBlockSize());
            proc.prepareToPlay (getSampleRate(), getBlockSize());
        };

        prepareInternal (*pluginInstance);

        const auto description = factory->createPluginDescription (valueOrRef);
        auto effect = new EffectProcessor (std::move (pluginInstance), description);

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
                prepareInternal (*proc);

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

    Logger::writeToLog (String ("EffectProcessorChain: moving index ABC to XYZ")
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

std::optional<juce::Point<int>> EffectProcessorChain::getLastUIPosition (int index) const
{
    return getEffectProperty<juce::Point<int>> (index, [&] (EffectProcessor::Ptr e) { return e->lastUIPosition; });
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
        newRequiredChannels = jmax (newRequiredChannels,
                                    effect->description.numInputChannels,
                                    effect->description.numOutputChannels);

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

    auto prepareInternal = [&] (AudioProcessor& proc)
    {
        proc.setPlayHead (getPlayHead());
        proc.setProcessingPrecision (getProcessingPrecision());
        proc.setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);
        proc.prepareToPlay (sampleRate, estimatedSamplesPerBlock);
    };

    for (int i = 0; i < effects.size(); ++i)
    {
        if (i >= effectLevels.size())
            if (auto* proc = effectLevels.add (new LevelsProcessor()))
                prepareInternal (*proc);

        if (auto effect = effects.getUnchecked (i))
            if (auto plugin = effect->plugin)
                prepareInternal (*plugin);
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
namespace ChainIds
{
    #define CREATE_ATTRIBUTE(name) \
        static const String name = JUCE_STRINGIFY (name);

    CREATE_ATTRIBUTE (rootBypassed)
    CREATE_ATTRIBUTE (effectRoot)
    CREATE_ATTRIBUTE (effectName)
    CREATE_ATTRIBUTE (effectBypassed)
    CREATE_ATTRIBUTE (effectMixLevel)
    CREATE_ATTRIBUTE (effectUIX)
    CREATE_ATTRIBUTE (effectUIY)
    CREATE_ATTRIBUTE (effectState)

    #undef CREATE_ATTRIBUTE
}

void EffectProcessorChain::getStateInformation (MemoryBlock& destData)
{
    SQUAREPINE_CRASH_TRACER

    Logger::writeToLog ("EffectProcessorChain: giving the state information to the host.");

    XmlElement effectChainElement (getIdentifier().toString());
    effectChainElement.setAttribute (ChainIds::rootBypassed, isBypassed() ? 1 : 0);

    for (auto effect : effects)
        if (effect != nullptr)
            effectChainElement.addChildElement (toXml (effect));

    AudioProcessor::copyXmlToBinary (effectChainElement, destData);
}

XmlElement* EffectProcessorChain::toXml (EffectProcessor::Ptr effect)
{
    SQUAREPINE_CRASH_TRACER

    if (effect == nullptr)
    {
        jassertfalse;
        return nullptr;
    }

    auto* effectElement = new XmlElement (ChainIds::effectRoot);
    effectElement->setAttribute (ChainIds::effectName, effect->getName());
    effectElement->setAttribute (ChainIds::effectBypassed, effect->isBypassed() ? 1 : 0);
    effectElement->setAttribute (ChainIds::effectMixLevel, effect->getMixLevel());
    effectElement->setAttribute (ChainIds::effectUIX, effect->lastUIPosition.x);
    effectElement->setAttribute (ChainIds::effectUIY, effect->lastUIPosition.y);
    effectElement->addChildElement (effect->description.createXml().release());

    {
        MemoryBlock m;
        effect->plugin->getStateInformation (m);
        effectElement->createNewChildElement (ChainIds::effectState)->addTextElement (Base64::toBase64 (m.getData(), m.getSize()));
    }

    return effectElement;
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
    clear();

    auto chainElement = AudioProcessor::getXmlFromBinary (data, sizeInBytes);

    if (chainElement != nullptr && chainElement->hasTagName (getIdentifier().toString()))
    {
        setBypassed (chainElement->getBoolAttribute (ChainIds::rootBypassed));

        for (auto* e : chainElement->getChildWithTagNameIterator (ChainIds::effectRoot))
            effects.add (createEffectProcessorFromXML (e));

        updateLatency();
        Logger::writeToLog ("EffectProcessorChain: succeeded!");
    }
    else
    {
        Logger::writeToLog ("EffectProcessorChain: state setting failed - bad data.");
        setBypassed (false); // To reset back to a normal state
        jassertfalse;
    }
}

EffectProcessor::Ptr EffectProcessorChain::createEffectProcessorFromXML (XmlElement* const effectXML)
{
    SQUAREPINE_CRASH_TRACER

    if (effectXML == nullptr)
    {
        jassertfalse;
        return {};
    }

    auto* pdState = effectXML->getChildByName ("PLUGIN");
    if (pdState == nullptr)
    {
        jassertfalse;
        return {};
    }

    PluginDescription description;
    if (! description.loadFromXml (*pdState))
    {
        jassertfalse;
        return {};
    }

    if (auto newEffect = insertInternal (-1, description, InsertionStyle::append))
    {
        MemoryBlock data;
        newEffect->plugin->getStateInformation (data);
        newEffect->setDefaultState (data);
        newEffect->setLastState (data);

        if (const auto* const state = effectXML->getChildByName (ChainIds::effectState))
        {
            MemoryOutputStream mos (data, false);
            if (Base64::convertFromBase64 (mos, state->getAllSubText()))
                newEffect->setLastState (data);
        }

        newEffect->setName (effectXML->getStringAttribute (ChainIds::effectName, String()).trim());
        newEffect->setMixLevel ((float) effectXML->getDoubleAttribute (ChainIds::effectMixLevel, 1.0));
        newEffect->setBypassed (effectXML->getBoolAttribute (ChainIds::effectBypassed));
        newEffect->lastUIPosition.x = effectXML->getIntAttribute (ChainIds::effectUIX);
        newEffect->lastUIPosition.y = effectXML->getIntAttribute (ChainIds::effectUIY);
        newEffect->reloadFromStateIfValid();
        return newEffect;
    }

    return {};
}
