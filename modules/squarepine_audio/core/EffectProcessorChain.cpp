//==============================================================================
EffectProcessorChain::EffectProcessorChain (EffectProcessorFactory::Ptr epf) :
    factory (epf)
{
    SQUAREPINE_CRASH_TRACER

    jassert (factory != nullptr);
    plugins.ensureStorageAllocated (8);
}

//==============================================================================
template<typename Type>
EffectProcessor::Ptr EffectProcessorChain::insertInternal (int destinationIndex, const Type& valueOrRef, InsertionStyle insertionStyle)
{
    SQUAREPINE_CRASH_TRACER

    if (factory == nullptr)
    {
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

        if (auto* proc = effectLevels.add (new LevelsProcessor()))
            prepareInternal (*proc);

        prepareInternal (*pluginInstance);

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

EffectProcessor::Ptr EffectProcessorChain::add (int s)                          { return insertInternal (-1, s, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::add (const String& s)                { return insertInternal (-1, s, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, int s)             { return insertInternal (dest, s); }
EffectProcessor::Ptr EffectProcessorChain::insert (int dest, const String& s)   { return insertInternal (dest, s); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, int s)            { return insertInternal (dest, s, InsertionStyle::replace); }
EffectProcessor::Ptr EffectProcessorChain::replace (int dest, const String& s)  { return insertInternal (dest, s, InsertionStyle::replace); }

void EffectProcessorChain::move (int pluginIndex, int destinationIndex)
{
    SQUAREPINE_CRASH_TRACER

    plugins.swap (pluginIndex, destinationIndex);
}

int EffectProcessorChain::getNumEffects() const
{
    return plugins.size();
}

bool EffectProcessorChain::remove (int index)
{
    SQUAREPINE_CRASH_TRACER

    const auto startSize = getNumEffects();
    plugins.remove (index);
    return startSize != getNumEffects();
}

bool EffectProcessorChain::clear()
{
    SQUAREPINE_CRASH_TRACER

    const bool changed = ! plugins.isEmpty();
    if (changed)
    {
        plugins.clear();
        updateLatency();
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
        if (auto effect = plugins[index])
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

    for (auto effect : plugins)
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
    for (auto effect : plugins)
        if (auto plugin = effect->plugin)
            newLatency += plugin->getLatencySamples();

    setLatencySamples (newLatency);
}

void EffectProcessorChain::prepareToPlay (double sampleRate, int estimatedSamplesPerBlock)
{
    SQUAREPINE_CRASH_TRACER

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

    for (int i = 0; i < plugins.size(); ++i)
    {
        if (i >= effectLevels.size())
            if (auto* proc = effectLevels.add (new LevelsProcessor()))
                prepareInternal (*proc);

        if (auto effect = plugins.getUnchecked (i))
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

    for (auto effect : plugins)
    {
        if (effect == nullptr)
            continue;

        // Process the effect:
        bufferPackage.effectBuffer.clear();
        addFrom (bufferPackage.effectBuffer, bufferPackage.mixingBuffer, numChannels, numSamples);

        if (! effect->canBeProcessed())
            bufferPackage.effectBuffer.clear();
        else
            effect->plugin->processBlock (bufferPackage.effectBuffer, midiMessages);

        // Add the effect-saturated samples at the specified mix level:
        const auto mixLevel = effect->mixLevel.getNextValue();
        jassert (isPositiveAndBelow (mixLevel, 1.00001f));

        bufferPackage.lastBuffer.clear();
        addFrom (bufferPackage.lastBuffer, bufferPackage.effectBuffer, numChannels, numSamples, mixLevel);

        // Add the original samples, at a percentage of the original gain, if the effect level isn't 100%:
        if (mixLevel < 1.0f)
            addFrom (bufferPackage.lastBuffer, bufferPackage.mixingBuffer, numChannels, numSamples, mixLevel);

        // Copy the result:
        bufferPackage.mixingBuffer.clear();
        addFrom (bufferPackage.mixingBuffer, bufferPackage.lastBuffer, numChannels, numSamples);

        effectLevels.getUnchecked (plugins.indexOf (effect))
            ->processBlock (bufferPackage.mixingBuffer, midiMessages);
    }

    source.clear();
    addFrom (source, bufferPackage.mixingBuffer, numChannels, numSamples);
}

bool EffectProcessorChain::isWholeChainBypassed() const
{
    int numBypassedPlugins = 0;

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
    SQUAREPINE_CRASH_TRACER

    const ScopedNoDenormals snd;

    if (isBypassed())
        return; // Pass through

    const auto numChannels = jmin (buffer.getNumChannels(), requiredChannels.load());
    const auto maxNumChannels = jmax (buffer.getNumChannels(), requiredChannels.load());
    const auto numSamples = buffer.getNumSamples();

    if (! plugins.isEmpty()
        && numChannels > 0
        && numSamples > 0
        && ! isWholeChainBypassed())
    {
        processInternal (buffer, midiMessages, package,
                         numChannels, maxNumChannels, numSamples);
    }
}

void EffectProcessorChain::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)  { process<float> (buffer, midiMessages, floatBuffers); }
void EffectProcessorChain::processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer& midiMessages) { process<double> (buffer, midiMessages, doubleBuffers); }

//==============================================================================
double EffectProcessorChain::getTailLengthSeconds() const
{
    SQUAREPINE_CRASH_TRACER

    auto largestTailLength = 0.0;

    for (auto effect : plugins)
        if (effect != nullptr && effect->canBeProcessed())
            if (auto plugin = effect->plugin)
                largestTailLength = jmax (largestTailLength, plugin->getTailLengthSeconds());

    return largestTailLength;
}

void EffectProcessorChain::setNonRealtime (bool isNonRealtime) noexcept
{
    SQUAREPINE_CRASH_TRACER

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
    SQUAREPINE_CRASH_TRACER

    const ScopedBypass sb (*this);

    clear();
    InternalProcessor::setBypassed (false); // To reset back to a normal state

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
        InternalProcessor::setBypassed (chainElement->getBoolAttribute (ChainIds::rootBypassed));

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
