EffectProcessor::EffectProcessor (std::shared_ptr<AudioPluginInstance> api,
                            const PluginDescription& pd) :
    isBypassed (false),
    mixLevel (1.0f),
    plugin (std::move (api)),
    description (pd)
{
    if (plugin != nullptr)
    {
        name = plugin->getName();

        // Capture the plugin's state at start-up in case the developers
        // haven't implemented the getParameterDefaultValue interface
        plugin->getStateInformation (defaultState);
    }

    mixLevel.setCurrentAndTargetValue (1.0f);
}

bool EffectProcessor::reloadFromStateIfValid()
{
    if (! isMissing())
    {
        bool wasValid = false;

        MemoryOutputStream stream;

        if (Base64::convertFromBase64 (stream, lastKnownBase64State)
            && stream.getData() != nullptr
            && stream.getDataSize() > 0)
        {
            plugin->setStateInformation (stream.getData(), (int) stream.getDataSize());
            wasValid = true;
        }

        lastKnownBase64State.clear();
        return wasValid;
    }

    return true;
}

bool EffectProcessor::canBeProcessed() const noexcept
{
    return ! isMissing()
        && ! isBypassed.load (std::memory_order_relaxed)
        && mixLevel.getTargetValue() > 0.0f;
}
