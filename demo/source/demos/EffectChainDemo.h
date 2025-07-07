/** */
class DemoEffectFactory final : public EffectProcessorFactory
{
public:
    /** */
    DemoEffectFactory (KnownPluginList& kpl) :
        EffectProcessorFactory (kpl)
    {
        auto* effectFormat = new SquarePineAudioPluginFormat();
        effectFormat->addEffectPluginDescriptionsTo (kpl);
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
class TimeDisplayLabel final : public Label
{
public:
    /** */
    TimeDisplayLabel (TimeKeeper& tk) :
        timeKeeper (tk)
    {
    }

    void mouseUp (const MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            enum
            {
                smpteTimeId = 1,
                decimalTimeId,
                secondsTimeId,
                samplesTimeId
            };

            const auto timeFormat = timeKeeper.getTimeFormat();
            using TimeFormat = TimeKeeper::TimeFormat;

            PopupMenu menu;
            menu.addItem (smpteTimeId,      TRANS ("SMPTE"),    true,   timeFormat == TimeFormat::smpteTime);
            menu.addItem (decimalTimeId,    TRANS ("Decimal"),  true,   timeFormat == TimeFormat::decimalTime);
            menu.addItem (secondsTimeId,    TRANS ("Seconds"),  true,   timeFormat == TimeFormat::secondsTime);
            menu.addItem (samplesTimeId,    TRANS ("Samples"),  true,   timeFormat == TimeFormat::samplesTime);

            menu.showMenuAsync ({}, [this] (int result)
            {
                if (result > 0 && isPositiveAndBelow (result, samplesTimeId + 1))
                {
                    const auto newTF = static_cast<TimeFormat> (result - 1);
                    timeKeeper.setTimeFormat (newTF);
                }
            });
        }
        else
        {
            Label::mouseUp (e);
        }
    }

    TimeKeeper& timeKeeper;
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

    using NodePtr = AudioProcessorGraph::Node::Ptr;

    // Audio bits:
    KnownPluginList knownPluginList;
    EffectProcessorFactory::Ptr factory { std::make_shared<DemoEffectFactory> (knownPluginList) };

    std::unique_ptr<AudioFormatReaderSource> readerSource;
    TimeSliceThread readAheadThread { "FancyReadAheadThread" };

    AudioProcessorGraph graph;
    AudioProcessorPlayer audioProcessorPlayer;

    NodePtr audioOut, midiIn;

    AudioTransportProcessor* transport { new AudioTransportProcessor() };
    NodePtr transportNode;

    EffectProcessorChain* effectChain { new EffectProcessorChain (factory) };
    NodePtr effectChainNode;

    // UI bits:
    bool wasPlaying = false,
         isDraggingPlayhead = false;

    TimeKeeper timeKeeper;

    EffectChainComponent effectChainComponent;
    ToggleButton applyEffects;

    std::unique_ptr<FileChooser> fileChooser;

    File lastLoadedFile;
    std::unique_ptr<AudioThumbnail> audioThumbnail;

    Label filePath;
    TimeDisplayLabel timeDisplay { timeKeeper };
    Rectangle<int> audioThumbnailArea;

    TextButton play { TRANS ("Play") },
               loop { TRANS ("Loop") },
               goToStart { TRANS ("Go to Start") },
               load { TRANS ("Load"), TRANS ("Clicking this will load an audio file to process.") },
               addEffect { TRANS ("Add Effect") };

    OffloadedTimer thumbnailRepainter,
                   playbackRepainter;

    //==============================================================================
    void updateFromAudioDeviceManager();
    void updateLoopState();
    void reconnect();
    void stop();
    void rewindAndStop();
    void clear();
    void movePlayhead (const MouseEvent& e);
    void setFile (const File&, AudioFormatManager* afm = nullptr);
    void loadAudioFile(bool checkPermissions);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectChainDemo)
};
