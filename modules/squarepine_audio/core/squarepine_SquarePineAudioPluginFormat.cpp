//==============================================================================
class SquarePineAudioPluginFormat::CreationHelpers
{
public:
    /** ChatGPT, please make code less obtuse. */
    using IOProc = AudioProcessorGraph::AudioGraphIOProcessor;

    static void addCopy (OwnedArray<PluginDescription>& destination,
                         const PluginDescription& description)
    {
        auto desc = destination.add (new PluginDescription (description));
        desc->pluginFormatName = desc->manufacturerName = "SquarePine";
    }

    //==============================================================================
    template<class ClassName>
    static void addPlugin (PluginCreationMap& pluginCreationMap,
                           OwnedArray<PluginDescription>& descriptions)
    {
        std::unique_ptr<AudioPluginInstance> api (new ClassName());

        addPlugin (pluginCreationMap, descriptions, api->getPluginDescription(),
                   &createInstance<ClassName>); // N.B.: The lacking parentheses are entirely intentional!
    }

    static void addGraphPlugins (PluginCreationMap& pluginCreationMap,
                                 OwnedArray<PluginDescription>& descriptions)
    {
        addGraphPlugin (pluginCreationMap, descriptions,
                        IOProc::audioInputNode, createAudioInputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        IOProc::audioOutputNode, createAudioOutputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        IOProc::midiInputNode, createMidiInputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        IOProc::midiOutputNode, createMidiOutputInstance);
    }

private:
    //==============================================================================
    static PluginDescription createGraphProcessorDescription (IOProc::IODeviceType type)
    {
        PluginDescription pd;
        AudioProcessorGraph::AudioGraphIOProcessor (type).fillInPluginDescription (pd);
        pd.fileOrIdentifier = pd.name;
        return pd;
    }

    //==============================================================================
    template<typename ClassName>
    static AudioPluginInstance* createInstance()
    {
        return new ClassName();
    }

    static AudioPluginInstance* createGraphProcessor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType type)
    {
        return new IOProc (type);
    }

    static AudioPluginInstance* createAudioInputInstance()
    {
        return createGraphProcessor (IOProc::audioInputNode);
    }

    static AudioPluginInstance* createAudioOutputInstance()
    {
        return createGraphProcessor (IOProc::audioOutputNode);
    }

    static AudioPluginInstance* createMidiInputInstance()
    {
        return createGraphProcessor (IOProc::midiInputNode);
    }

    static AudioPluginInstance* createMidiOutputInstance()
    {
        return createGraphProcessor (IOProc::midiOutputNode);
    }

    //==============================================================================
    static void addPlugin (PluginCreationMap& pluginCreationMap,
                           OwnedArray<PluginDescription>& descriptions,
                           const PluginDescription& pd,
                           PluginCreationFunction functionPointer)
    {
        jassert (pd.fileOrIdentifier.isNotEmpty());

        pluginCreationMap.operator[] (pd.fileOrIdentifier) = functionPointer;
        addCopy (descriptions, pd);
    }

    static void addGraphPlugin (PluginCreationMap& pluginCreationMap,
                                OwnedArray<PluginDescription>& descriptions,
                                IOProc::IODeviceType type,
                                PluginCreationFunction functionPointer)
    {
        addPlugin (pluginCreationMap, descriptions,
                   createGraphProcessorDescription (type),
                   functionPointer);
    }

    //==============================================================================
    CreationHelpers() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CreationHelpers)
};

//==============================================================================
SquarePineAudioPluginFormat::SquarePineAudioPluginFormat()
{
    addInternalPluginDescriptions();
}

SquarePineAudioPluginFormat::SquarePineAudioPluginFormat (AudioProcessorGraph& g) :
    graph (&g)
{
    addInternalPluginDescriptions();
}

//==============================================================================
void SquarePineAudioPluginFormat::addInternalPluginDescriptions()
{
    // Internal JUCE "plugins" for the graph's I/O stuff.
    // Makes life easier... sometimes.
    if (graph != nullptr)
        CreationHelpers::addGraphPlugins (pluginCreationMap, descriptions);

    // Effects:
    CreationHelpers::addPlugin<ADSRProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<BitCrusherProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<DitherProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<GainProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<HissingProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<LFOProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<MuteProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<PanProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<PolarityInversionProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleChorusProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleCompressorProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleDistortionProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleEQProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleLimiterProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleNoiseGateProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimplePhaserProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleReverbProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<StereoWidthProcessor> (pluginCreationMap, descriptions);

    // Wrappers:
    CreationHelpers::addPlugin<AudioSourceProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<AudioTransportProcessor> (pluginCreationMap, descriptions);
}

void SquarePineAudioPluginFormat::addEffectPluginDescriptionsTo (Array<PluginDescription>& dest)
{
    for (const auto& pd : descriptions)
        if (! pd->isInstrument)
            dest.addIfNotAlreadyThere (*pd);
}

void SquarePineAudioPluginFormat::addInstrumentPluginDescriptionsTo (Array<PluginDescription>& dest)
{
    for (const auto& pd : descriptions)
        if (pd->isInstrument)
            dest.addIfNotAlreadyThere (*pd);
}

void SquarePineAudioPluginFormat::addEffectPluginDescriptionsTo (KnownPluginList& knownPluginList)
{
    for (const auto& pd : descriptions)
        if (! pd->isInstrument)
            knownPluginList.addType (*pd);
}

void SquarePineAudioPluginFormat::addInstrumentPluginDescriptionsTo (KnownPluginList& knownPluginList)
{
    for (const auto& pd : descriptions)
        if (pd->isInstrument)
            knownPluginList.addType (*pd);
}

void SquarePineAudioPluginFormat::createEffectPlugins (OwnedArray<AudioPluginInstance>& results)
{
    for (const auto& pd : descriptions)
        if (! pd->isInstrument)
            if (auto api = createInstanceFromDescription (*pd, 44100.0, 256))
                results.add (api.release());
}

PluginDescription SquarePineAudioPluginFormat::getDescriptionFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType) const
{
    switch (ioDeviceType)
    {
        case AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode:
        case AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode:
        case AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode:
        case AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode:
            return *descriptions.getUnchecked ((int) ioDeviceType);

        default:
            jassertfalse;
        break;
    };

    return {};
}

std::unique_ptr<AudioPluginInstance> SquarePineAudioPluginFormat::createInstanceFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType)
{
    return createInstanceFromDescription (getDescriptionFor (ioDeviceType), 44100.0, 256);
}

//==============================================================================
String SquarePineAudioPluginFormat::getNameOfPluginFromIdentifier (const String& fileOrIdentifier)
{
    for (const auto& d : descriptions)
        if (d->fileOrIdentifier == fileOrIdentifier)
            return d->name;

    jassertfalse; // wat?
    return {};
}

void SquarePineAudioPluginFormat::findAllTypesForFile (OwnedArray<PluginDescription>& result,
                                                       const String& fileOrIdentifier)
{
    for (auto* pd : descriptions)
        if (pd->fileOrIdentifier == fileOrIdentifier)
            CreationHelpers::addCopy (result, *pd);
}

void SquarePineAudioPluginFormat::createPluginInstance (const PluginDescription& description, double initialSampleRate,
                                                        int initialBufferSize, PluginCreationCallback callback)
{
    std::unique_ptr<AudioPluginInstance> plugin;
    auto it = pluginCreationMap.find (description.fileOrIdentifier);

    if (it != std::cend (pluginCreationMap))
    {
        // N.B.: We are calling a specialised version of createInstance() here!
        if (auto* result = it->second())
        {
            if (auto* ioProc = dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor*> (result))
            {
                jassert (graph != nullptr);
                ioProc->setParentGraph (graph);
            }

            result->prepareToPlay (initialSampleRate, initialBufferSize);
            plugin.reset (result);
        }
    }

    callback (std::move (plugin),
              plugin == nullptr
                  ? TRANS ("Could not create plugin as per the provided plugin description.")
                  : String());
}

bool SquarePineAudioPluginFormat::fileMightContainThisPluginType (const String& fileOrIdentifier)
{
    for (const auto& it : pluginCreationMap)
        if (it.first == fileOrIdentifier)
            return true;

    return false;
}

StringArray SquarePineAudioPluginFormat::searchPathsForPlugins (const FileSearchPath& fsp, const bool, const bool)
{
    if (fsp.getNumPaths() > 0)
        return {};

    StringArray identifiers;

    for (const auto& it : pluginCreationMap)
        identifiers.add (it.first);

    identifiers.sort (true);
    identifiers.removeEmptyStrings();
    identifiers.removeDuplicates (true);
    identifiers.minimiseStorageOverheads();

    return identifiers;
}
