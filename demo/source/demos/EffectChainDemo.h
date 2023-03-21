/** */
class DemoEffectFormat final : public AudioPluginFormat
{
public:
    /** */
    DemoEffectFormat()
    {
    }

    //==============================================================================
    /** */
    void fill (KnownPluginList& kpl)
    {
        addPlugin<BitCrusherProcessor> (kpl);
        addPlugin<DitherProcessor> (kpl);
        addPlugin<GainProcessor> (kpl);
        addPlugin<HissingProcessor> (kpl);
        addPlugin<LFOProcessor> (kpl);
        addPlugin<MuteProcessor> (kpl);
        addPlugin<PanProcessor> (kpl);
        addPlugin<PolarityInversionProcessor> (kpl);
        addPlugin<SimpleChorusProcessor> (kpl);
        addPlugin<SimpleCompressorProcessor> (kpl);
        addPlugin<SimpleDistortionProcessor> (kpl);
        addPlugin<SimpleEQProcessor> (kpl);
        addPlugin<SimpleLimiterProcessor> (kpl);
        addPlugin<SimpleNoiseGateProcessor> (kpl);
        addPlugin<SimplePhaserProcessor> (kpl);
        addPlugin<SimpleReverbProcessor> (kpl);
        addPlugin<StereoWidthProcessor> (kpl);
    }

    //==============================================================================
    /** @internal */
    String getName() const override                                                             { return "SquarePine"; }
    /** @internal */
    bool isTrivialToScan() const override                                                       { return true; }
    /** @internal */
    bool canScanForPlugins() const override                                                     { return false; }
    /** @internal */
    bool pluginNeedsRescanning (const PluginDescription&) override                              { return false; }
    /** @internal */
    bool doesPluginStillExist (const PluginDescription&) override                               { return true; }
    /** @internal */
    StringArray searchPathsForPlugins (const FileSearchPath&, bool, bool) override              { return {}; }
    /** @internal */
    FileSearchPath getDefaultLocationsToSearch() override                                       { return {}; }
    /** @internal */
    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const override { return false; }

    /** @internal */
    void findAllTypesForFile (OwnedArray<PluginDescription>& results,
                              const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                results.add (new PluginDescription (ed->description));
    }

    /** @internal */
    bool fileMightContainThisPluginType (const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                return true;

        return false;
    }

    /** @internal */
    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                return TRANS (ed->description.name);

        return {};
    }

    /** @internal */
    void createPluginInstance (const PluginDescription& description, double initialSampleRate,
                               int initialBufferSize, PluginCreationCallback callback) override
    {
        jassert (callback != nullptr);

        for (const auto& ed : effectDetails)
        {
            if (ed->description.fileOrIdentifier == description.fileOrIdentifier)
            {
                auto plugin = ed->createInstance();
                jassert (plugin != nullptr);

                plugin->prepareToPlay (initialSampleRate, initialBufferSize);
                callback (std::move (plugin), {});
                return;
            }
        }

        callback (nullptr, TRANS ("Failed!"));
    }

private:
    //==============================================================================
    struct EffectDetails final
    {
        EffectDetails() = default;

        template<typename PluginClass>
        void init (StringRef formatName)
        {
            static_assert (std::is_base_of_v<InternalProcessor, PluginClass>);

            description = PluginClass().getPluginDescription();
            description.pluginFormatName = formatName;

            createInstance = []()
            {
                return std::make_unique<PluginClass> ();
            };
        }

        PluginDescription description;
        std::function<std::unique_ptr<AudioPluginInstance> ()> createInstance;
    };

    OwnedArray<EffectDetails> effectDetails;

    //==============================================================================
    template<typename PluginClass>
    void addPlugin (KnownPluginList& kpl)
    {
        auto* ed = effectDetails.add (new EffectDetails());
        ed->init<PluginClass> (getName());
        kpl.addType (ed->description);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoEffectFormat)
};

//==============================================================================
/** */
class DemoEffectFactory final : public EffectProcessorFactory
{
public:
    /** */
    DemoEffectFactory (KnownPluginList& kpl) :
        EffectProcessorFactory (kpl)
    {
        auto* effectFormat = new DemoEffectFormat();
        effectFormat->fill (kpl);
        apfm.addFormat (effectFormat);
    }

    //==============================================================================
    /** @internal */
    const AudioPluginFormatManager& getAudioPluginFormatManager() const override { return apfm; }

private:
    //==============================================================================
    AudioPluginFormatManager apfm;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoEffectFactory)
};

//==============================================================================
class EffectChainComponent;
class EffectChainDemo;

//==============================================================================
class EditorWindow final : public DocumentWindow
{
public:
    /** */
    EditorWindow (EffectChainComponent&, StringRef effectName, EffectProcessor::Ptr);

    //==============================================================================
    /** */
    EffectProcessor::Ptr getEffect() const { return effect; }

    //==============================================================================
    /** @internal */
    void closeButtonPressed() override;

private:
    //==============================================================================
    EffectChainComponent& owner;
    EffectProcessor::Ptr effect;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EditorWindow)
};

//==============================================================================
/** */
class EffectRowComponent final : public Component
{
public:
    /** */
    EffectRowComponent (EffectChainComponent&);

    //==============================================================================
    /** */
    EffectRowComponent& setIndex (int);
    /** */
    EffectRowComponent& setEffect (EffectProcessor::Ptr);
    /** */
    void deleteEffect();
    /** */
    void moveEffect (int newIndex);
    /** */
    void closeWindow();

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    void paint (Graphics&) override;
    /** @internal */
    void mouseDown (const MouseEvent&) override;
    /** @internal */
    void mouseDoubleClick (const MouseEvent&) override;

private:
    //==============================================================================
    EffectChainComponent& owner;
    EffectProcessor::Ptr effect;
    int index = -1;
    Slider mixLevel;
    ToggleButton active;
    Label name;
    SafePointer<EditorWindow> editorWindow;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectRowComponent)
};

//==============================================================================
/** */
class EffectChainComponent final : public Component,
                                   public ListBoxModel,
                                   public AudioProcessorListener
{
public:
    /** */
    EffectChainComponent (SharedObjects&, EffectProcessorChain&);
    /** */
    ~EffectChainComponent() override;

    //==============================================================================
    /** */
    void deleteEffect (int index, bool shouldUpdateContent);

    //==============================================================================
    /** @internal */
    int getNumRows() override                                       { return effectChain.getNumEffects(); }
    /** @internal */
    bool mayDragToExternalWindows() const override                  { return false; }
    /** @internal */
    void paintListBoxItem (int, Graphics&, int, int, bool) override { }
    /** @internal */
    void backgroundClicked (const MouseEvent&) override             { listbox.deselectAllRows(); }
    /** @internal */
    void selectedRowsChanged (int) override                         { }
    /** @internal */
    void returnKeyPressed (int) override                            { listbox.deselectAllRows(); }
    /** @internal */
    void resized() override                                         { listbox.setBounds (getLocalBounds()); }
    /** @internal */
    Component* refreshComponentForRow (int, bool, Component*) override;
    /** @internal */
    void deleteKeyPressed (int) override;
    /** @internal */
    void audioProcessorParameterChanged (AudioProcessor*, int, float) override;
    /** @internal */
    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override;

private:
    //==============================================================================
    friend EditorWindow;
    friend EffectRowComponent;

    SharedObjects& sharedObjects;
    EffectProcessorChain& effectChain;
    OwnedArray<EditorWindow> editorWindows;
    ListBox listbox;
    OffloadedTimer contentUpdateTimer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectChainComponent)
};

//==============================================================================
/** */
class EffectChainDemo final : public DemoBase,
                              public ChangeListener
{
public:
    /** */
    EffectChainDemo (SharedObjects&);

    /** */
    ~EffectChainDemo() override;

    //==============================================================================
    /** @internal */
    void changeListenerCallback (ChangeBroadcaster*) override;
    /** @internal */
    void mouseDown (const MouseEvent&) override;
    /** @internal */
    void mouseDrag (const MouseEvent&) override;
    /** @internal */
    void mouseUp (const MouseEvent&) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void paint (Graphics&) override;

private:
    //==============================================================================
    friend EffectRowComponent;
    friend EffectChainComponent;

    using Node = AudioProcessorGraph::Node::Ptr;

    // Audio bits:
    KnownPluginList knownPluginList;
    EffectProcessorFactory::Ptr factory { std::make_shared<DemoEffectFactory> (knownPluginList) };

    std::unique_ptr<AudioFormatReaderSource> readerSource;
    TimeSliceThread readAheadThread { "FancyReadAheadThread" };

    AudioProcessorGraph graph;
    AudioProcessorPlayer audioProcessorPlayer;

    Node audioOut, midiIn;

    AudioTransportProcessor* transport { new AudioTransportProcessor() };
    Node transportNode;

    EffectProcessorChain* effectChain { new EffectProcessorChain (factory) };
    Node effectChainNode;

    // UI bits:
    bool wasPlaying = false,
         isDraggingPlayhead = false;

    File lastLoadedFile;
    std::unique_ptr<AudioThumbnail> audioThumbnail;

    Label filePath, timeDisplay;
    Rectangle<int> audioThumbnailArea;

    TextButton play { TRANS ("Play") },
               loop { TRANS ("Loop") },
               goToStart { TRANS ("Go to Start") },
               load { TRANS ("Load"), TRANS ("Clicking this will load an audio file to process.") },
               addEffect { TRANS ("Add Effect") };

    TimeKeeper timeKeeper;

    EffectChainComponent effectChainComponent;
    ToggleButton applyEffects;

    OffloadedTimer thumbnailRepainter,
                   playbackRepainter;

    std::unique_ptr<FileChooser> chooser;

    //==============================================================================
    void updateFromAudioDeviceManager();
    void updateLoopState();
    void reconnect();
    void stop();
    void rewindAndStop();
    void clear();
    void movePlayhead (const MouseEvent& e);
    void setFile (const File&, AudioFormatManager* afm = nullptr);
    void loadAudioFile();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectChainDemo)
};
