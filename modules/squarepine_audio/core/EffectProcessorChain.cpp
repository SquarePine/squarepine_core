//==============================================================================
EffectProcessorChain::EffectProcessorChain (std::shared_ptr<EffectProcessorFactory> epf) :
    factory (epf)
{
    jassert (factory != nullptr);
    plugins.reserve (10);
}

//==============================================================================
template<typename Type>
EffectProcessor::Ptr EffectProcessorChain::insertInternal (const Type& valueOrRef, int destinationIndex, InsertionStyle insertionStyle)
{
    if (insertionStyle != InsertionStyle::append
        && ! isPositiveAndBelow (destinationIndex, getNumEffects()))
    {
        jassertfalse;
        return {};
    }

    if (factory == nullptr)
    {
        jassertfalse;
        return {};
    }

    if (auto pluginInstance = factory->createPlugin (valueOrRef))
    {
        pluginInstance->setPlayHead (getPlayHead());
        pluginInstance->prepareToPlay (getSampleRate(), getBlockSize());

        auto effect = std::make_shared<EffectProcessor> (std::move (pluginInstance), factory->createPluginDescription (valueOrRef));

        {
            const ScopedLock sl (getCallbackLock());

            if (insertionStyle == InsertionStyle::insert)
                plugins.emplace_back (effect);
            else if (insertionStyle == InsertionStyle::append)
                plugins.emplace_back (effect);
            else
                plugins.emplace_back (effect);

            updateLatency();
        }

        updateHostDisplay();
        return effect;
    }

    jassertfalse;
    return {};
}

EffectProcessor::Ptr EffectProcessorChain::appendNewEffect (int source)                     { return insertInternal (source, -1, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::appendNewEffect (const String& source)           { return insertInternal (source, -1, InsertionStyle::append); }
EffectProcessor::Ptr EffectProcessorChain::insertNewEffect (int source, int dest)           { return insertInternal (source, dest); }
EffectProcessor::Ptr EffectProcessorChain::insertNewEffect (const String& source, int dest) { return insertInternal (source, dest); }
EffectProcessor::Ptr EffectProcessorChain::replaceEffect (int source, int dest)             { return insertInternal (source, dest, InsertionStyle::replace); }
EffectProcessor::Ptr EffectProcessorChain::replaceEffect (const String& source, int dest)   { return insertInternal (source, dest, InsertionStyle::replace); }

//==============================================================================
bool EffectProcessorChain::moveEffect (int pluginIndex, int destinationIndex)
{
    bool changed = false;

    {
        const ScopedLock sl (getCallbackLock());
        changed = moveItem (plugins, pluginIndex, std::clamp (destinationIndex, 0, getNumEffects()));
    }

    if (changed)
        updateHostDisplay();

    return changed;
}

bool EffectProcessorChain::moveEffect (int pluginIndex, PluginPositionPreset destinationPosition)
{
    switch (destinationPosition)
    {
        case PluginPositionPreset::shiftToPrevious: return moveEffect (pluginIndex, pluginIndex - 1);
        case PluginPositionPreset::shiftToNext:     return moveEffect (pluginIndex, pluginIndex + 1);
        default: break;
    };

    bool changed = false;

    {
        const ScopedLock sl (getCallbackLock());

        switch (destinationPosition)
        {
            case PluginPositionPreset::shiftToFirst:    changed = moveItemToFront (plugins, pluginIndex); break;
            case PluginPositionPreset::shiftToLast:     changed = moveItemToBack (plugins, pluginIndex); break;
            default:                                    jassertfalse; break;
        };
    }

    if (changed)
        updateHostDisplay();

    return changed;
}

//==============================================================================
int EffectProcessorChain::getNumEffects() const
{
    const ScopedLock sl (getCallbackLock());
    return static_cast<int> (plugins.size());
}

bool EffectProcessorChain::removeEffect (int index)
{
    bool changed = false;

    {
        const ScopedLock sl (getCallbackLock());
        changed = removeItem (plugins, index);
        updateLatency();
    }

    if (changed)
        updateHostDisplay();

    return changed;
}

bool EffectProcessorChain::clear()
{
    bool changed = false;

    {
        const ScopedLock sl (getCallbackLock());
        changed = ! plugins.empty();

        if (changed)
        {
            plugins.clear();
            updateLatency(); // Doing this here to avoid doubly locking.
        }
    }

    if (changed)
        updateHostDisplay();

    return changed;
}

//==============================================================================
bool EffectProcessorChain::setEffectName (int index, const String& name)
{
    {
        const ScopedLock sl (getCallbackLock());

        if (isPositiveAndBelow (index, getNumEffects()))
        {
            if (auto effect = plugins[(size_t) index])
            {
                effect->name = name;
                return true;
            }
        }
    }

    return false;
}

bool EffectProcessorChain::setBypass (int index, bool bypass)
{
    {
        const ScopedLock sl (getCallbackLock());

        if (isPositiveAndBelow (index, getNumEffects()))
        {
            if (auto effect = plugins[(size_t) index])
            {
                effect->isBypassed = bypass;
                return true;
            }
        }
    }

    return false;
}

bool EffectProcessorChain::setMixLevel (int index, float mixLevel)
{
    {
        const ScopedLock sl (getCallbackLock());

        if (isPositiveAndBelow (index, getNumEffects()))
        {
            if (auto effect = plugins[(size_t) index])
            {
                effect->mixLevel = mixLevel;
                return true;
            }
        }
    }

    return false;
}

//==============================================================================
SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (String) EffectProcessorChain::getEffectName (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins.at ((size_t) index))
            return { effect->name };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (String) EffectProcessorChain::getPluginInstanceName (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            if (auto p = effect->plugin)
                return { p->getName() };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (std::shared_ptr<AudioPluginInstance>) EffectProcessorChain::getPluginInstance (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->plugin };

    return {};
}

EffectProcessor::Ptr EffectProcessorChain::getEffectProcessor (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return effect;

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (PluginDescription) EffectProcessorChain::getPluginDescription (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->description };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (bool) EffectProcessorChain::isBypassed (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->isBypassed.load (std::memory_order_relaxed) };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (float) EffectProcessorChain::getMixLevel (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->mixLevel.getTargetValue() };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (juce::Point<int>) EffectProcessorChain::getLastUIPosition (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->lastUIPosition };

    return {};
}

SQUAREPINE_OPTIONALLY_OPTIONAL_TYPE (bool) EffectProcessorChain::isPluginMissing (int index) const
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects()))
        if (auto effect = plugins[(size_t) index])
            return { effect->isMissing() };

    return {};
}

bool EffectProcessorChain::loadIfMissing (int index)
{
    const ScopedLock sl (getCallbackLock());

    if (isPositiveAndBelow (index, getNumEffects())
        && isPluginMissing (index))
    {
        if (auto effect = plugins[(size_t) index])
        {
            const InternalProcessor::ScopedBypass sb (*this);

            effect->plugin = factory->createPlugin (effect->description);
            effect->reloadFromStateIfValid();
            return true;
        }
    }

    return false;
}

//==============================================================================
void EffectProcessorChain::prepareToPlay (const double sampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);

    const auto numChans = jmax (getTotalNumInputChannels(), getTotalNumOutputChannels(), 1);

    const ScopedLock sl (getCallbackLock());

    floatBuffers.update (numChans, estimatedSamplesPerBlock);
    doubleBuffers.update (numChans, estimatedSamplesPerBlock);

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

    // N.B.: This probably isn't accurate at all (@todo ?)
    for (auto effect : plugins)
        if (auto plugin = effect->plugin)
            setLatencySamples (getLatencySamples() + plugin->getLatencySamples());
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
    // Uses requiredChannels to ensure enough memory is allocated for the plugin to potentially read from/write to - avoids bad accesses
    // Currently we only care about the main outputs, so still use the numChannels variable throughout
    // @todo: Add support for aux/bus/side chain channels
    bufferPackage.update (requiredChannels, numSamples);
    bufferPackage.clear();

    const auto channels = jmin (numChannels, requiredChannels.load());

    addFrom (bufferPackage.mixingBuffer, source, channels, numSamples);

    for (auto effect : plugins)
    {
        if (effect == nullptr || ! effect->canBeProcessed())
            continue;

        //Process the effect:
        bufferPackage.effectBuffer.clear();
        addFrom (bufferPackage.effectBuffer, bufferPackage.mixingBuffer, channels, numSamples);

        processSafely (*effect->plugin, bufferPackage.effectBuffer, midiMessages);

        //Add the effect-saturated samples at the specified mix level:
        const auto mixLevel = effect->mixLevel.getNextValue();
        jassert (isPositiveAndBelow (mixLevel, 1.00001f));

        bufferPackage.lastBuffer.clear();
        addFrom (bufferPackage.lastBuffer, bufferPackage.effectBuffer, channels, numSamples, mixLevel);

        //Add the original samples, at a percentage of the original gain, if the effect level isn't 100%:
        if (mixLevel < 1.0f)
            addFrom (bufferPackage.lastBuffer, bufferPackage.mixingBuffer, channels, numSamples, mixLevel);

        //Copy the result:
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
void EffectProcessorChain::process (juce::AudioBuffer<FloatType>& buffer, MidiBuffer& midiMessages, BufferPackage<FloatType>& package)
{
    if (InternalProcessor::isBypassed())
        return;

    const GenericScopedTryLock<CriticalSection> sl (getCallbackLock());
    const auto numChannels = jmin ((int) 2, buffer.getNumChannels());
    const auto numSamples = buffer.getNumSamples();

    if (sl.isLocked()
        && ! plugins.empty()
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
    const ScopedLock sl (getCallbackLock());
    auto largestTailLength = 0.0;

    for (auto effect : plugins)
        if (effect != nullptr && effect->canBeProcessed())
            if (auto plugin = effect->plugin)
                largestTailLength = jmax (largestTailLength, plugin->getTailLengthSeconds());

    return largestTailLength;
}

void EffectProcessorChain::releaseResources()           { loopThroughEffectsAndCall<&AudioProcessor::releaseResources>(); }
void EffectProcessorChain::reset()                      { loopThroughEffectsAndCall<&AudioProcessor::reset>(); }
void EffectProcessorChain::numChannelsChanged()         { loopThroughEffectsAndCall<&AudioProcessor::numChannelsChanged>(); }
void EffectProcessorChain::numBusesChanged()            { loopThroughEffectsAndCall<&AudioProcessor::numBusesChanged>(); }
void EffectProcessorChain::processorLayoutsChanged()    { loopThroughEffectsAndCall<&AudioProcessor::processorLayoutsChanged>(); }
const String EffectProcessorChain::getName() const      { return TRANS ("Effect Processor Chain"); }
Identifier EffectProcessorChain::getIdentifier() const  { return "EffectProcessorChain"; }
bool EffectProcessorChain::acceptsMidi() const          { return true; }
bool EffectProcessorChain::producesMidi() const         { return true; }

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
    effectElement->setAttribute (ChainIds::effectName, effect->name);
    effectElement->setAttribute (ChainIds::effectBypassed, effect->isBypassed ? 1 : 0);
    effectElement->setAttribute (ChainIds::effectMixLevel, std::clamp (effect->mixLevel.getTargetValue(), 0.0f, 1.0f));
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
        /** All supported 3rd-party plugin formats (ie: VST2, VST3, AU, RTAS),
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
            createEffectProcessorFromXML (e);

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

    if (auto newEffect = insertInternal (description, -1, InsertionStyle::append))
    {
        newEffect->plugin->getStateInformation (newEffect->defaultState);

        if (const auto* const state = effectXML->getChildByName (ChainIds::effectState))
        {
            newEffect->lastKnownBase64State = state->getAllSubText();
            newEffect->reloadFromStateIfValid();
        }

        newEffect->name = effectXML->getStringAttribute (ChainIds::effectName, String()).trim();
        newEffect->mixLevel = (float) std::clamp (effectXML->getDoubleAttribute (ChainIds::effectMixLevel, 1.0), 0.0, 1.0);
        newEffect->isBypassed = effectXML->getBoolAttribute (ChainIds::effectBypassed);
        newEffect->lastUIPosition.x = effectXML->getIntAttribute (ChainIds::effectUIX);
        newEffect->lastUIPosition.y = effectXML->getIntAttribute (ChainIds::effectUIY);
        return newEffect;
    }

    return {};
}
