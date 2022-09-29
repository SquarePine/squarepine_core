/** */
class EffectChainDemo final : public DemoBase
{
public:
    /** */
    EffectChainDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Effect Chain"))
    {
        clear();

        sharedObjects.audioDeviceManager.addAudioCallback (&audioSourcePlayer);
        audioSourcePlayer.setSource (&audioTransportSource);

        play.onClick = [&]()
        {
            if (! play.isEnabled())
                rewindAndStop();
            else if (audioTransportSource.isPlaying())
                audioTransportSource.stop();
            else
                audioTransportSource.start();
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
            if (audioTransportSource.isPlaying())
                repaint();
        };

        addAndMakeVisible (play);
        addAndMakeVisible (goToStart);
        addAndMakeVisible (load);
        addAndMakeVisible (filePath);
    }

    /** */
    ~EffectChainDemo() override
    {
        sharedObjects.audioDeviceManager.removeAudioCallback (&audioSourcePlayer);
        audioSourcePlayer.setSource (nullptr);
    }

    //==============================================================================
    /** */
    void mouseDown (const MouseEvent& e) override
    {
        wasPlaying = audioTransportSource.isPlaying();
        audioTransportSource.stop();
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
            audioTransportSource.start();

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
        const auto timeSeconds = audioTransportSource.getCurrentPosition();

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
    class EffectSource final : public AudioSource
    {
    public:
        EffectSource()
        {
        }

        ~EffectSource()
        {
        }

        void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
        {
        }

        void releaseResources() override
        {
        }

        void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
        {
        }

        std::unique_ptr<EffectProcessorChain> chain;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectSource)
    };

    std::unique_ptr<FileChooser> chooser;

    KnownPluginList internalPluginsList;
    std::shared_ptr<EffectProcessorFactory> factory;

    File lastLoadedFile;
    std::unique_ptr<AudioThumbnail> audioThumbnail;

    std::unique_ptr<AudioFormatReaderSource> readerSource;
    TimeSliceThread readAheadThread { "FancyReadAheadThread" };
    AudioTransportSource audioTransportSource;
    AudioSourcePlayer audioSourcePlayer;

    bool wasPlaying = false,
         isDraggingPlayhead = false;

    Label filePath;
    Rectangle<int> audioThumbnailArea;

    TextButton play { TRANS ("Play") },
               goToStart { TRANS ("Go to Start") },
               load { TRANS ("Load"), TRANS ("Clicking this will load an audio file to process.") };

    OffloadedTimer thumbnailRepainter,
                   playbackRepainter;

    //==============================================================================
    void stopTransports()
    {
        audioTransportSource.stop();
    }

    void rewindAndStop()
    {
        stopTransports();
        audioTransportSource.setPosition (0.0);
        repaint();
    }

    void clear()
    {
        rewindAndStop();
        audioTransportSource.setSource (nullptr);
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

        audioTransportSource.setPosition (timeSeconds);
    }

    void setFile (const File& file, AudioFormatManager* audioFormatManager = nullptr)
    {
        clear();

        if (! file.existsAsFile() || audioFormatManager == nullptr)
            return;

        if (auto* reader = audioFormatManager->createReaderFor (file))
        {
            readerSource.reset (new AudioFormatReaderSource (reader, true));

            audioTransportSource.setSource (readerSource.get(), 0, nullptr,
                                            sharedObjects.audioDeviceManager.getAudioDeviceSetup().sampleRate);
            play.setEnabled (true);
            goToStart.setEnabled (true);
        }
    }

    void loadAudioFile()
    {
        if (chooser != nullptr)
            return;

        stopTransports();

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
