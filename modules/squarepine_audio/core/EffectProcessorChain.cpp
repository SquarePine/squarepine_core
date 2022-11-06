//==============================================================================
EffectProcessorChain::EffectProcessorChain (std::shared_ptr<EffectProcessorFactory> epf) :
    factory (epf)
{
    jassert (factory != nullptr);
    plugins.ensureStorageAllocated (8);
}

//==============================================================================
template<typename Type>
EffectProcessor::Ptr EffectProcessorChain::insertInternal (int destinationIndex, const Type& valueOrRef, InsertionStyle insertionStyle)
{
    if (factory == nullptr)
    {
        jassertfalse;
        return {};
    }

    if (auto pluginInstance = factory->createPlugin (valueOrRef))
    {
        pluginInstance->setPlayHead (getPlayHead());
        pluginInstance->prepareToPlay (getSampleRate(), getBlockSize());

        auto effect = new EffectProcessor (std::move (pluginInstance), factory->createPluginDescription (valueOrRef));

        if (insertionStyle == InsertionStyle::append
            || ! isPositiveAndBelow (destinationIndex, getNumEffects()))
        {
            plugins.add (effect);
        }
        else if (insertionStyle == InsertionStyle::insert)
        {
            plugins.insert (destinationIndex, effect);
        }
        else
        {
            plugins.set (destinationIndex, effect);
        }

        updateLatency();
        updateHostDisplay();
        return effect;
    }

    jassertfalse;
    return {};
}

EffectProcessor::Ptr EffectProcessorChain::add (int source)                         { return insertInternal (-1, source, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::add (const String& source)               { return insertInternal (-1, source, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, int source)            { return insertInternal (dest, source); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, const String& source)  { return insertInternal (dest, source); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, int source)           { return insertInternal (dest, source, InsertionStyle::replace); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, const String& source) { return insertInternal (dest, source, InsertionStyle::replace); }

void EffectProcessorChain::move (int pluginIndex, int destinationIndex)
{
    plugins.swap (pluginIndex, destinationIndex);
}

int EffectProcessorChain::getNumEffects() const
{
    return static_cast<int> (plugins.size());
}

bool EffectProcessorChain::remove (int index)
{
    const auto startSize = getNumEffects();
    plugins.remove (index);
    return startSize != getNumEffects();
}

bool EffectProcessorChain::clear()
{
    const bool changed = ! plugins.isEmpty();
    if (changed)
    {
        plugins.clear();
        updateLatency(); // Doing this here to avoid doubly locking.
    }

    if (changed)
        updateHostDisplay();

    return changed;
}

//==============================================================================
EffectProcessor::Ptr EffectProcessorChain::getEffectProcessor (int index) const
{
    if (isPositiveAndBelow (index, getNumEffects()))
        return plugins[index];

    return {};
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

std::optional<std::shared_ptr<AudioPluginInstance>> EffectProcessorChain::getPluginInstance (int index) const
{
    return getEffectProperty<std::shared_ptr<AudioPluginInstance>> (index, [&] (EffectProcessor::Ptr e) { return e->plugin; });
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
    if (isPositiveAndBelow (index, getNumEffects())
        && isPluginMissing (index))
    {
        if (auto effect = plugins[(size_t) index])
        {
            const InternalProcessor::ScopedBypass sb (*this);

            effect->plugin = factory->createPlugin (effect->description);
            return effect->reloadFromStateIfValid();
        }
    }

    return false;
}

//==============================================================================
bool EffectProcessorChain::setEffectProperty (int index, std::function<void (EffectProcessor::Ptr)> func)
{
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
void EffectProcessorChain::prepareToPlay (const double sampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels(), 1);

    floatBuffers.prepare (numChans, estimatedSamplesPerBlock);
    doubleBuffers.prepare (numChans, estimatedSamplesPerBlock);

    for (auto effect : plugins)
    {
        if (effect != nullptr)
        {
            if (auto plugin = effect->plugin)
            {
                plugin->setPlayHead (getPlayHead());
                plugin->prepareToPlay (sampleRate, estimatedSamplesPerBlock);
            }
        }
    }

    updateLatency();
}

void EffectProcessorChain::updateLatency()
{
    updateChannelCount();

    int newLatency = 0;

    for (auto effect : plugins)
        if (auto plugin = effect->plugin)
            newLatency += plugin->getLatencySamples();

    setLatencySamples (newLatency);
}

void EffectProcessorChain::updateChannelCount()
{
    int newRequiredChannels = 0;

    for (auto effect : plugins)
        newRequiredChannels = jmax (newRequiredChannels, effect->description.numInputChannels, effect->description.numOutputChannels);

    requiredChannels = newRequiredChannels;
}

//==============================================================================
template<typename FloatType>
void EffectProcessorChain::processInternal (juce::AudioBuffer<FloatType>& source,
                                            MidiBuffer& midiMessages,
                                            BufferPackage<FloatType>& bufferPackage,
                                            const int numChannels,
                                            const int numSamples)
{
    // Uses requiredChannels to ensure enough memory is allocated for the plugin to
    // potentially read from/write to - avoids bad accesses.
    // We only care about the main outputs, so still use the numChannels variable throughout
    bufferPackage.prepare (requiredChannels, numSamples);

    const auto channels = jmin (numChannels, requiredChannels.load());

    addFrom (bufferPackage.mixingBuffer, source, channels, numSamples);

    for (auto effect : plugins)
    {
        if (effect == nullptr || ! effect->canBeProcessed())
            continue;

        // Process the effect:
        bufferPackage.effectBuffer.clear();
        addFrom (bufferPackage.effectBuffer, bufferPackage.mixingBuffer, channels, numSamples);

        processSafely (*effect->plugin, bufferPackage.effectBuffer, midiMessages);

        // Add the effect-saturated samples at the specified mix level:
        const auto mixLevel = effect->mixLevel.getNextValue();
        jassert (isPositiveAndBelow (mixLevel, 1.00001f));

        bufferPackage.lastBuffer.clear();
        addFrom (bufferPackage.lastBuffer, bufferPackage.effectBuffer, channels, numSamples, mixLevel);

        // Add the original samples, at a percentage of the original gain, if the effect level isn't 100%:
        if (mixLevel < 1.0f)
            addFrom (bufferPackage.lastBuffer, bufferPackage.mixingBuffer, channels, numSamples, mixLevel);

        // Copy the result:
        bufferPackage.mixingBuffer.clear();
        addFrom (bufferPackage.mixingBuffer, bufferPackage.lastBuffer, channels, numSamples);
    }

    source.clear();
    addFrom (source, bufferPackage.mixingBuffer, channels, numSamples);
}

bool EffectProcessorChain::isWholeChainBypassed() const
{
    size_t numBypassedPlugins = 0;

    for (auto effect : plugins)
        if (effect == nullptr || ! effect->canBeProcessed())
            ++numBypassedPlugins;

    return numBypassedPlugins == plugins.size();
}

template<typename FloatType>
void EffectProcessorChain::process (juce::AudioBuffer<FloatType>& buffer,
                                    MidiBuffer& midiMessages,
                                    BufferPackage<FloatType>& package)
{
    const ScopedNoDenormals snd;

    if (InternalProcessor::isBypassed())
        return; // Pass through

    const auto numChannels = jmin ((int) 2, buffer.getNumChannels());
    const auto numSamples = buffer.getNumSamples();

    if (! plugins.isEmpty()
        && numChannels > 0
        && numSamples > 0
        && ! isWholeChainBypassed())
    {
        processInternal (buffer, midiMessages, package, numChannels, numSamples);
    }
}

void EffectProcessorChain::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)  { process<float> (buffer, midiMessages, floatBuffers); }
void EffectProcessorChain::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) { process<double> (buffer, midiMessages, doubleBuffers); }

//==============================================================================
double EffectProcessorChain::getTailLengthSeconds() const
{
    auto largestTailLength = 0.0;

    for (auto effect : plugins)
        if (effect != nullptr && effect->canBeProcessed())
            if (auto plugin = effect->plugin)
                largestTailLength = jmax (largestTailLength, plugin->getTailLengthSeconds());

    return largestTailLength;
}

void EffectProcessorChain::setNonRealtime (bool isNonRealtime) noexcept
{
    for (auto effect : plugins)
        if (effect != nullptr)
            if (auto plugin = effect->plugin)
                plugin->setNonRealtime (isNonRealtime);
}

void EffectProcessorChain::releaseResources()           { loopThroughEffectsAndCall<&AudioProcessor::releaseResources>(); }
void EffectProcessorChain::reset()                      { loopThroughEffectsAndCall<&AudioProcessor::reset>(); }
void EffectProcessorChain::numChannelsChanged()         { loopThroughEffectsAndCall<&AudioProcessor::numChannelsChanged>(); }
void EffectProcessorChain::numBusesChanged()            { loopThroughEffectsAndCall<&AudioProcessor::numBusesChanged>(); }
void EffectProcessorChain::processorLayoutsChanged()    { loopThroughEffectsAndCall<&AudioProcessor::processorLayoutsChanged>(); }
const String EffectProcessorChain::getName() const      { return NEEDS_TRANS ("Effect Processor Chain"); }

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
    XmlElement effectChainElement (getIdentifier().toString());
    effectChainElement.setAttribute (ChainIds::rootBypassed, InternalProcessor::isBypassed() ? 1 : 0);

    for (auto effect : plugins)
        if (effect != nullptr)
            effectChainElement.addChildElement (createElementForEffect (effect));

    AudioProcessor::copyXmlToBinary (effectChainElement, destData);
}

XmlElement* EffectProcessorChain::createElementForEffect (EffectProcessor::Ptr effect)
{
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
    clear();
    InternalProcessor::setBypass (false); // To reset back to a normal state

    const ScopedBypass sb (*this);

    if (! MessageManager::getInstance()->isThisTheMessageThread())
    {
        /** All supported 3rd-party plugin formats (eg: VST2, VST3, AU),
            for various reasons, require being created on the message thread!
            Restructure your code to make it so!
        */
        jassertfalse;
        return;
    }

    auto chainElement = AudioProcessor::getXmlFromBinary (data, sizeInBytes);

    if (chainElement != nullptr && chainElement->getTagName() == getIdentifier().toString())
    {
        InternalProcessor::setBypass (chainElement->getBoolAttribute (ChainIds::rootBypassed));

        for (auto* e : chainElement->getChildWithTagNameIterator (ChainIds::effectRoot))
            plugins.add (createEffectProcessorFromXML (e));

        updateLatency();
    }
    else
    {
        jassertfalse;
    }
}

EffectProcessor::Ptr EffectProcessorChain::createEffectProcessorFromXML (XmlElement* const effectXML)
{
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

        if (const auto* const state = effectXML->getChildByName (ChainIds::effectState))
        {
            // newEffect->lastKnownBase64State = state->getAllSubText();
            newEffect->reloadFromStateIfValid();
        }

        newEffect->setName (effectXML->getStringAttribute (ChainIds::effectName, String()).trim());
        newEffect->setMixLevel ((float) effectXML->getDoubleAttribute (ChainIds::effectMixLevel, 1.0));
        newEffect->setBypassed (effectXML->getBoolAttribute (ChainIds::effectBypassed));
        newEffect->lastUIPosition.x = effectXML->getIntAttribute (ChainIds::effectUIX);
        newEffect->lastUIPosition.y = effectXML->getIntAttribute (ChainIds::effectUIY);
        return newEffect;
    }

    return {};
}
