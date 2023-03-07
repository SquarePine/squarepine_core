EffectProcessorFactory::EffectProcessorFactory (KnownPluginList& kpl) :
    knownPluginList (kpl)
{
}

EffectProcessorFactory::~EffectProcessorFactory()
{
}

//==============================================================================
PluginDescription EffectProcessorFactory::createPluginDescription (const int pluginIndex) const
{
    return knownPluginList.getTypes()[pluginIndex];
}

PluginDescription EffectProcessorFactory::createPluginDescription (const String& fileOrIdentifier) const
{
    if (auto pd = knownPluginList.getTypeForIdentifierString (fileOrIdentifier))
        return *pd;

    if (auto pd = knownPluginList.getTypeForFile (fileOrIdentifier))
        return *pd;

    jassertfalse;
    return {};
}

PluginDescription EffectProcessorFactory::createPluginDescription (const PluginDescription& description) const
{
    return description;
}

//==============================================================================
AudioPluginPtr EffectProcessorFactory::createPlugin (const PluginDescription& description) const
{
    if (description.isInstrument)
        return nullptr;

    String errorMessage;
    return getAudioPluginFormatManager().createPluginInstance (description, 44100.0, 256, errorMessage);
}

AudioPluginPtr EffectProcessorFactory::createPlugin (const int listIndex) const
{
    return createPlugin (createPluginDescription (listIndex));
}

AudioPluginPtr EffectProcessorFactory::createPlugin (const String& fileOrIdentifier) const
{
    return createPlugin (createPluginDescription (fileOrIdentifier));
}

//==============================================================================
void EffectProcessorFactory::createPluginAsync (const PluginDescription& description, PluginCreationCallback callback)
{
    if (description.isInstrument)
        return;

    const_cast<AudioPluginFormatManager&> (getAudioPluginFormatManager())
        .createPluginInstanceAsync (description, 44100.0, 256,
            [&] (std::unique_ptr<AudioPluginInstance> api, const String& s)
            {
                if (callback != nullptr)
                    callback (std::move (api), s);
            });
}

void EffectProcessorFactory::createPluginAsync (int index, PluginCreationCallback callback)
{
    createPluginAsync (createPluginDescription (index), callback);
}

void EffectProcessorFactory::createPluginAsync (const String& fileOrIdentifier, PluginCreationCallback callback)
{
    createPluginAsync (createPluginDescription (fileOrIdentifier), callback);
}
