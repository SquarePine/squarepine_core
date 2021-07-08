//==============================================================================
class InternalAudioPluginFormat::CreationHelpers
{
public:
    static void addCopy (OwnedArray<PluginDescription>& destination,
                         const PluginDescription& description)
    {
        destination.add (new PluginDescription (description));
    }

    //==============================================================================
    template<class ClassName>
    static void addPlugin (PluginCreationMap& pluginCreationMap,
                           OwnedArray<PluginDescription>& descriptions)
    {
        std::unique_ptr<AudioPluginInstance> api (new ClassName());

        addPlugin (pluginCreationMap, descriptions, api->getPluginDescription(),
                   &createInstance<ClassName>); //N.B.: The lacking parentheses are entirely intentional!
    }

    static void addGraphPlugins (PluginCreationMap& pluginCreationMap,
                                 OwnedArray<PluginDescription>& descriptions)
    {
        addGraphPlugin (pluginCreationMap, descriptions,
                        AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode,
                        createAudioInputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode,
                        createAudioOutputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode,
                        createMidiInputInstance);

        addGraphPlugin (pluginCreationMap, descriptions,
                        AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode,
                        createMidiOutputInstance);
    }

private:
    //==============================================================================
    static PluginDescription createGraphProcessorDescription (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType type)
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
        return new AudioProcessorGraph::AudioGraphIOProcessor (type);
    }

    static AudioPluginInstance* createAudioInputInstance()
    {
        return createGraphProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode);
    }

    static AudioPluginInstance* createAudioOutputInstance()
    {
        return createGraphProcessor (AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode);
    }

    static AudioPluginInstance* createMidiInputInstance()
    {
        return createGraphProcessor (AudioProcessorGraph::AudioGraphIOProcessor::midiInputNode);
    }

    static AudioPluginInstance* createMidiOutputInstance()
    {
        return createGraphProcessor (AudioProcessorGraph::AudioGraphIOProcessor::midiOutputNode);
    }

    //==============================================================================
    static void addPlugin (PluginCreationMap& pluginCreationMap,
                           OwnedArray<PluginDescription>& descriptions,
                           const PluginDescription& pd,
                           PluginCreationFunction functionPointer)
    {
        jassert (pd.fileOrIdentifier.isNotEmpty());

        pluginCreationMap.operator[](pd.fileOrIdentifier) = functionPointer;
        addCopy (descriptions, pd);
    }

    static void addGraphPlugin (PluginCreationMap& pluginCreationMap,
                                OwnedArray<PluginDescription>& descriptions,
                                AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType type,
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
InternalAudioPluginFormat::InternalAudioPluginFormat (AudioProcessorGraph& g) :
    graph (g),
    numGraphPlugins (-1)
{
    addInternalPluginDescriptions();
}

//==============================================================================
void InternalAudioPluginFormat::addInternalPluginDescriptions()
{
    //Internal JUCE plugins:
    CreationHelpers::addGraphPlugins (pluginCreationMap, descriptions);
    numGraphPlugins = descriptions.size();

    //Effects:
    CreationHelpers::addPlugin<ADSRProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<BitCrusherProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<ChorusProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<DitherProcessor> (pluginCreationMap, descriptions);
    //CreationHelpers::addPlugin<EffectProcessorChain> (pluginCreationMap, descriptions);
    //CreationHelpers::addPlugin<JUCEReverbProcessor> (pluginCreationMap, descriptions);
    //CreationHelpers::addPlugin<LFOProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<MuteProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<PanProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<PolarityInversionProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<SimpleDistortionProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<StereoWidthProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<GainProcessor> (pluginCreationMap, descriptions);

    //Wrappers:
    CreationHelpers::addPlugin<AudioSourceProcessor> (pluginCreationMap, descriptions);
    CreationHelpers::addPlugin<AudioTransportProcessor> (pluginCreationMap, descriptions);
}

void InternalAudioPluginFormat::addPluginDescriptions (KnownPluginList& knownPluginList)
{
    for (auto* pd : descriptions)
        knownPluginList.addType (*pd);
}

void InternalAudioPluginFormat::createEffectPlugins (OwnedArray<AudioPluginInstance>& results)
{
    for (auto* pd : descriptions)
        if (! pd->isInstrument)
            if (auto api = createInstanceFromDescription (*pd, 44100.0, 256))
                results.add (api.release());
}

PluginDescription InternalAudioPluginFormat::getDescriptionFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType) const
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

std::unique_ptr<AudioPluginInstance> InternalAudioPluginFormat::createInstanceFor (AudioProcessorGraph::AudioGraphIOProcessor::IODeviceType ioDeviceType)
{
    return createInstanceFromDescription (getDescriptionFor (ioDeviceType), 44100.0, 256);
}

//==============================================================================
String InternalAudioPluginFormat::getName() const                                                                       { return getInternalProcessorTypeName(); }
bool InternalAudioPluginFormat::requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const noexcept  { return false; }
String InternalAudioPluginFormat::getNameOfPluginFromIdentifier (const String& fileOrIdentifier)                        { return fileOrIdentifier; }
bool InternalAudioPluginFormat::pluginNeedsRescanning (const PluginDescription&)                                        { return false; }
bool InternalAudioPluginFormat::doesPluginStillExist (const PluginDescription&)                                         { return true; }
bool InternalAudioPluginFormat::canScanForPlugins() const                                                               { return false; }
FileSearchPath InternalAudioPluginFormat::getDefaultLocationsToSearch()                                                 { return {}; }

void InternalAudioPluginFormat::findAllTypesForFile (OwnedArray<PluginDescription>& result,
                                                     const String& fileOrIdentifier)
{
    for (auto* pd : descriptions)
        if (pd->fileOrIdentifier == fileOrIdentifier)
            CreationHelpers::addCopy (result, *pd);
}

void InternalAudioPluginFormat::createPluginInstance (const PluginDescription& description, double initialSampleRate,
                                                      int initialBufferSize, PluginCreationCallback callback)
{
    std::unique_ptr<AudioPluginInstance> plugin;
    auto it = pluginCreationMap.find (description.fileOrIdentifier);

    if (it != pluginCreationMap.end())
    {
        //N.B.: We are calling a specialised version of createInstance() here!
        if (auto* result = it->second())
        {
            if (auto* ioProc = dynamic_cast<AudioProcessorGraph::AudioGraphIOProcessor*> (result))
                ioProc->setParentGraph (&graph);

            result->prepareToPlay (initialSampleRate, initialBufferSize);
            plugin.reset (result);
        }
    }

    callback (std::move (plugin),
              plugin == nullptr
                  ? TRANS ("Could not create plugin as per the provided plugin description.")
                  : String());
}

bool InternalAudioPluginFormat::fileMightContainThisPluginType (const String& fileOrIdentifier)
{
    for (auto& it : pluginCreationMap)
        if (it.first == fileOrIdentifier)
            return true;

    return false;
}

StringArray InternalAudioPluginFormat::searchPathsForPlugins (const FileSearchPath&, const bool, const bool)
{
    StringArray identifiers;

    for (auto& it : pluginCreationMap)
        identifiers.addIfNotAlreadyThere (it.first);

    identifiers.sort (true);
    identifiers.minimiseStorageOverheads();

    return identifiers;
}
