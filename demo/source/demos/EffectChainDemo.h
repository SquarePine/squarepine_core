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
        addPlugin<ChorusProcessor> (kpl);
        addPlugin<DitherProcessor> (kpl);
        addPlugin<GainProcessor> (kpl);
        addPlugin<HissingProcessor> (kpl);
        addPlugin<JUCEReverbProcessor> (kpl);
        addPlugin<MuteProcessor> (kpl);
        addPlugin<PanProcessor> (kpl);
        addPlugin<PolarityInversionProcessor> (kpl);
        addPlugin<SimpleDistortionProcessor> (kpl);
        addPlugin<SimpleEQProcessor> (kpl);
        addPlugin<StereoWidthProcessor> (kpl);
    }

    //==============================================================================
    String getName() const override                                                             { return "SquarePine"; }
    bool isTrivialToScan() const override                                                       { return true; }
    bool canScanForPlugins() const override                                                     { return false; }
    bool pluginNeedsRescanning (const PluginDescription&) override                              { return false; }
    bool doesPluginStillExist (const PluginDescription&) override                               { return true; }
    StringArray searchPathsForPlugins (const FileSearchPath&, bool, bool) override              { return {}; }
    FileSearchPath getDefaultLocationsToSearch() override                                       { return {}; }
    bool requiresUnblockedMessageThreadDuringCreation (const PluginDescription&) const override { return false; }

    void findAllTypesForFile (OwnedArray<PluginDescription>& results,
                              const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                results.add (new PluginDescription (ed->description));
    }

    bool fileMightContainThisPluginType (const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                return true;

        return false;
    }

    String getNameOfPluginFromIdentifier (const String& fileOrIdentifier) override
    {
        for (const auto& ed : effectDetails)
            if (ed->description.fileOrIdentifier == fileOrIdentifier)
                return TRANS (ed->description.name);

        return {};
    }

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
    /** */
    const AudioPluginFormatManager& getAudioPluginFormatManager() const override { return apfm; }

private:
    //==============================================================================
    AudioPluginFormatManager apfm;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DemoEffectFactory)
};

//==============================================================================
/** */
class EffectChainComponent final : public Component,
                                   public ListBoxModel,
                                   public AudioProcessorListener
{
public:
    EffectChainComponent (EffectProcessorChain& epc) :
        effectChain (epc)
    {
        contentUpdateTimer.callback = [this]()
        {
            listbox.updateContent();
            contentUpdateTimer.stopTimer();
        };

        listbox.setModel (this);
        addAndMakeVisible (listbox);

        effectChain.addListener (this);
    }

    ~EffectChainComponent() override
    {
        effectChain.removeListener (this);
    }

    int getNumRows() override                                       { return effectChain.getNumEffects(); }
    void paintListBoxItem (int, Graphics&, int, int, bool) override { }
    void resized() override                                         { listbox.setBounds (getLocalBounds()); }

    void audioProcessorParameterChanged (AudioProcessor*, int, float) override
    {
        contentUpdateTimer.startTimer (1000);
    }

    void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override
    {
        contentUpdateTimer.startTimer (1000);
    }

    Component* refreshComponentForRow (int row, bool, Component* comp) override
    {
       #if JUCE_DEBUG && ! JUCE_DISABLE_ASSERTIONS
        if (comp != nullptr)
            jassert (dynamic_cast<EffectRowComponent*> (comp) != nullptr);
       #endif

        std::unique_ptr<EffectRowComponent> erc (static_cast<EffectRowComponent*> (comp));
        comp = nullptr;

        if (! isPositiveAndBelow (row, getNumRows()))
            return nullptr;

        auto effectProc = effectChain.getEffectProcessor (row);

        if (erc == nullptr)
            erc.reset (new EffectRowComponent());

        erc->setText (TRANS (effectProc->plugin->getPluginDescription().name), sendNotification);
        return erc.release();
    }

private:
    using EffectRowComponent = Label;

    EffectProcessorChain& effectChain;
    ListBox listbox;
    OffloadedTimer contentUpdateTimer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectChainComponent)
};

//==============================================================================
/** */
class EffectChainDemo final : public DemoBase
{
public:
    /** */
    EffectChainDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Effect Chain"))
    {
        clear();

        sharedObjects.audioDeviceManager.addAudioCallback (&audioProcessorPlayer);
        audioProcessorPlayer.setProcessor (&graph);

        play.onClick = [&]()
        {
            if (! play.isEnabled())
                rewindAndStop();
            else if (transport->isPlaying())
                transport->stop();
            else
                transport->play();
        };

        play.addShortcut (KeyPress (KeyPress::spaceKey));
        load.addShortcut (KeyPress::createFromDescription ("CMD + O"));
        goToStart.addShortcut (KeyPress (KeyPress::homeKey));

        load.onClick = [&]() { loadAudioFile(); };
        goToStart.onClick = [&]() { rewindAndStop(); };

        filePath.setJustificationType (Justification::centred);
        filePath.setColour (Label::backgroundColourId, Colours::transparentBlack);
        filePath.setColour (Label::textColourId, Colours::black);

        thumbnailRepainter.callback = [this]()
        {
            repaint();

            if (audioThumbnail == nullptr || audioThumbnail->isFullyLoaded())
                thumbnailRepainter.stopTimer();
        };

        playbackRepainter.callback = [this]()
        {
            if (transport->isPlaying())
                repaint();
        };

        using GraphProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
        auto addGraphPlugin = [&] (GraphProcessor::IODeviceType type)
        {
            return graph.addNode (std::make_unique<GraphProcessor> (type));
        };

        audioOut        = addGraphPlugin (GraphProcessor::audioOutputNode);
        midiIn          = addGraphPlugin (GraphProcessor::midiInputNode);
        transportNode   = graph.addNode (std::unique_ptr<AudioProcessor> (transport));
        effectChainNode = graph.addNode (std::unique_ptr<AudioProcessor> (effectChain));

        reconnect();

        for (const auto& pd : knownPluginList.getTypes())
            effectChain->appendNewEffect (pd.fileOrIdentifier);

        addAndMakeVisible (play);
        addAndMakeVisible (goToStart);
        addAndMakeVisible (load);
        addAndMakeVisible (filePath);
    }

    /** */
    ~EffectChainDemo() override
    {
        sharedObjects.audioDeviceManager.removeAudioCallback (&audioProcessorPlayer);
        audioProcessorPlayer.setProcessor (nullptr);
    }

    //==============================================================================
    /** */
    void mouseDown (const MouseEvent& e) override
    {
        wasPlaying = transport->isPlaying();
        transport->stop();
        movePlayhead (e);
    }

    /** */
    void mouseDrag (const MouseEvent& e) override
    {
        if (audioThumbnailArea.isEmpty() || audioThumbnail == nullptr)
            return;

        isDraggingPlayhead = true;
        movePlayhead (e);
        repaint();
    }

    /** */
    void mouseUp (const MouseEvent&) override
    {
        if (wasPlaying)
            transport->play();

        wasPlaying = false;
        isDraggingPlayhead = false;
    }

    /** */
    void resized() override
    {
        constexpr auto margin = 4;

        auto b = getLocalBounds().reduced (margin);

        {
            auto topArea = b.removeFromTop (32);
            const auto w = (topArea.getWidth() / 3) - (margin * 2);

            load.setBounds (topArea.removeFromLeft (w));
            play.setBounds (topArea.removeFromRight (w));

            topArea.removeFromLeft (margin);
            topArea.removeFromRight (margin);

            goToStart.setBounds (topArea);
        }

        b.removeFromTop (margin);
        filePath.setBounds (b.removeFromTop (32));

        b.removeFromTop (margin);
        audioThumbnailArea = b;
    }

    /** */
    void paint (Graphics& g) override
    {
        if (audioThumbnailArea.isEmpty() || audioThumbnail == nullptr)
            return;

        const auto lengthSeconds = audioThumbnail->getTotalLength();
        const auto timeSeconds = transport->getCurrentTimeSeconds();

        g.setColour (Colours::darkgrey);
        audioThumbnail->drawChannels (g, audioThumbnailArea, 0.0, lengthSeconds, 0.9f);

        g.setColour (Colours::dodgerblue);
        g.fillRect (audioThumbnailArea
                        .toDouble()
                        .withX ((int) jmap (timeSeconds, 0.0, lengthSeconds,
                                            (double) audioThumbnailArea.getX(),
                                            (double) audioThumbnailArea.getWidth()))
                        .withWidth (2.5)
                        .toFloat());
    }

private:
    //==============================================================================
    using Node = AudioProcessorGraph::Node::Ptr;

    // Audio bits:
    KnownPluginList knownPluginList;
    std::shared_ptr<EffectProcessorFactory> factory { std::make_shared<DemoEffectFactory> (knownPluginList) };

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

    Label filePath;
    Rectangle<int> audioThumbnailArea;

    TextButton play { TRANS ("Play") },
               goToStart { TRANS ("Go to Start") },
               load { TRANS ("Load"), TRANS ("Clicking this will load an audio file to process.") };

    EffectChainComponent effectChainComponent { *effectChain };

    OffloadedTimer thumbnailRepainter,
                   playbackRepainter;

    std::unique_ptr<FileChooser> chooser;

    //==============================================================================
    void reconnect()
    {
        auto nodes = graph.getNodes();

        for (const auto& node : graph.getNodes())
            graph.disconnectNode (node->nodeID);

        auto connect = [&] (Node& source, Node& dest, bool isMidi = false)
        {
            auto addConnection = [&] (int channelIndex)
            {
                // This trash heap of a graph node/connection API
                // is tedious at best, so let's try to somewhat
                // reduce the cognitive load...
                using NAC = AudioProcessorGraph::NodeAndChannel;

                NAC sourceNAC;
                sourceNAC.nodeID = source->nodeID;
                sourceNAC.channelIndex = channelIndex;

                NAC destNAC;
                destNAC.nodeID = dest->nodeID;
                destNAC.channelIndex = channelIndex;

                return graph.addConnection ({ sourceNAC, destNAC });
            };

            bool succeeded = false;
            if (isMidi)
                succeeded = addConnection (AudioProcessorGraph::midiChannelIndex);
            else
                succeeded = addConnection (0) && addConnection (1);

            jassert (succeeded);
            return succeeded;
        };

        connect (midiIn, effectChainNode, true);
        connect (transportNode, effectChainNode);
        connect (effectChainNode, audioOut);
    }

    void stop()
    {
        transport->stop();
    }

    void rewindAndStop()
    {
        stop();
        transport->setCurrentTime (0.0);
        repaint();
    }

    void clear()
    {
        rewindAndStop();
        transport->clear();
        readerSource.reset();
        play.setEnabled (false);
        goToStart.setEnabled (false);
    }

    void movePlayhead (const MouseEvent& e)
    {
        if (audioThumbnailArea.isEmpty() || audioThumbnail == nullptr)
            return;

        const auto mousePos = audioThumbnailArea.getConstrainedPoint (e.getMouseDownPosition());
        const auto timeSeconds = jmap (mousePos.toDouble().x,
                                       (double) audioThumbnailArea.getX(),
                                       (double) audioThumbnailArea.getWidth(),
                                       0.0, audioThumbnail->getTotalLength());

        transport->setCurrentTime (timeSeconds);
    }

    void setFile (const File& file, AudioFormatManager* audioFormatManager = nullptr)
    {
        clear();

        if (! file.existsAsFile() || audioFormatManager == nullptr)
            return;

        if (auto* reader = audioFormatManager->createReaderFor (file))
        {
            readerSource.reset (new AudioFormatReaderSource (reader, true));

            transport->setSource (readerSource.get(), 0, nullptr,
                                  sharedObjects.audioDeviceManager.getAudioDeviceSetup().sampleRate);
            play.setEnabled (true);
            goToStart.setEnabled (true);
        }
    }

    void loadAudioFile()
    {
        if (chooser != nullptr)
            return;

        stop();

        chooser.reset (new FileChooser (TRANS ("Load an audio file to process."),
                                        File::getSpecialLocation (File::userMusicDirectory),
                                        sharedObjects.audioFormatManager.getWildcardForAllFormats()));

        const auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

        chooser->launchAsync (folderChooserFlags, [this] (const FileChooser& fc)
        {
            auto c = std::move (chooser);
            ignoreUnused (c);

            const auto newFileURL = fc.getURLResult();
            if (newFileURL.isEmpty())
                return; // User cancelled.

            jassert (newFileURL.isLocalFile());

            const auto newFile = newFileURL.getLocalFile();
            if (lastLoadedFile == newFile)
                return;

            auto* reader = sharedObjects.audioFormatManager.createReaderFor (newFile);
            if (reader == nullptr)
            {
                NativeMessageBox::showAsync (MessageBoxOptions()
                                                .withIconType (AlertWindow::WarningIcon)
                                                .withTitle (TRANS ("Failed to read audio file!"))
                                                .withMessage (TRANS ("It seems the audio file has an unknown or incompatible codec...")),
                                             nullptr);
                return;
            }

            playbackRepainter.stopTimer();
            clear();

            lastLoadedFile = newFile;

            setFile (lastLoadedFile, &sharedObjects.audioFormatManager);

            String s;
            s
                << lastLoadedFile.getFullPathName()
                << newLine
                << reader->getFormatName()
                << ", Sample Rate: " << (int) reader->sampleRate
                << ", Num Channels: " << (int) reader->numChannels
                << ", Bit Depth: " << (int) reader->bitsPerSample;

            filePath.setText (s, sendNotificationAsync);

            if (audioThumbnail == nullptr)
                audioThumbnail.reset (new AudioThumbnail (2048,
                                                          sharedObjects.audioFormatManager,
                                                          sharedObjects.audioThumbnailCache));

            audioThumbnail->setReader (reader, (int64) reader);

            thumbnailRepainter.startTimerHz (60);
            playbackRepainter.startTimerHz (60);
        });
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectChainDemo)
};
