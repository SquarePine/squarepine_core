//==============================================================================
EditorWindow::EditorWindow (EffectChainComponent& o, StringRef effectName, EffectProcessor::Ptr e) :
    DocumentWindow (effectName, Colours::black, DocumentWindow::TitleBarButtons::allButtons, false),
    owner (o),
    effect (e)
{
}

void EditorWindow::closeButtonPressed()
{
    owner.editorWindows.removeObject (this);
}

//==============================================================================
EffectRowComponent::EffectRowComponent (EffectChainComponent& ecc) :
    owner (ecc)
{
    active.setClickingTogglesState (true);
    active.onStateChange = [this]()
    {
        const auto toggled = active.getToggleState();

        if (effect != nullptr)
            effect->setBypassed (! toggled, nullptr);

        String pluginInvalid;
        if (effect != nullptr && effect->getPlugin() == nullptr)
            pluginInvalid = TRANS (" (Please reload plugin)");

        if (effect == nullptr)
            active.setButtonText (TRANS ("(Please reload or replace effect)"));
        else if (toggled)
            active.setButtonText (TRANS ("Active") + pluginInvalid);
        else
            active.setButtonText (TRANS ("Bypassed") + pluginInvalid);
    };

    name.setJustificationType (Justification::centredLeft);
    name.setInterceptsMouseClicks (false, false);

    addAndMakeVisible (active);
    addAndMakeVisible (name);
}

EffectRowComponent& EffectRowComponent::setIndex (int newIndex)
{
    if (index != newIndex)
    {
        index = newIndex;
        repaint();
    }

    return *this;
}

EffectRowComponent& EffectRowComponent::setEffect (EffectProcessor::Ptr epp)
{
    if (effect != epp)
    {
        closeWindow();
        effect = epp;

        String newName ("/shrug");

        if (effect != nullptr)
        {
            newName = epp->getName();

            active.setToggleState (! effect->isBypassed(), dontSendNotification);
        }

        name.setText (newName, sendNotification);
        active.setVisible (effect != nullptr);
        active.onStateChange();

        resized();
    }

    return *this;
}

void EffectRowComponent::closeWindow()
{
    if (effect != nullptr)
        for (int i = owner.editorWindows.size(); --i >= 0;)
            if (auto* ew = owner.editorWindows.getUnchecked (i))
                if (ew->getEffect() == effect)
                    ew->closeButtonPressed();
}

void EffectRowComponent::deleteEffect()
{
    owner.deleteEffect (index, true);
}

void EffectRowComponent::moveEffect (int newIndex)
{
    owner.effectChain.move (index, newIndex);
    owner.listbox.updateContent();
}

void EffectRowComponent::resized()
{
    auto b = getLocalBounds();

    const auto sq = b.getHeight();

    if (active.isVisible())
        active.setBounds (b.removeFromLeft (sq * 3));

    name.setBounds (b);
}

void EffectRowComponent::paint (Graphics& g)
{
    if (effect != nullptr && owner.listbox.isRowSelected (index))
        g.fillAll (Colours::darkblue.withAlpha (0.5f));
}

void EffectRowComponent::mouseDown (const MouseEvent& e)
{
    owner.listbox.selectRowsBasedOnModifierKeys (index, e.mods, false);

    const auto numEffects = owner.effectChain.getNumEffects();

    if (e.mods.isPopupMenu()
        && isPositiveAndBelow (index, numEffects))
    {
        PopupMenu menu;

        enum
        {
            deleteId = -100,
            selectAllId,
            moveDownId,
            moveUpId,
            moveToTopId,
            moveToBottomId
        };

        if (numEffects > 0)
            menu.addItem (selectAllId, TRANS ("Select All"), true, false, Image());

        if (index > 0)
        {
            menu.addItem (moveUpId, TRANS ("Move Up"), true, false, Image());
            menu.addItem (moveToTopId, TRANS ("Move to Top"), true, false, Image());
        }

        if (index < (numEffects - 1))
        {
            menu.addItem (moveDownId, TRANS ("Move Down"), true, false, Image());
            menu.addItem (moveToBottomId, TRANS ("Move to Bottom"), true, false, Image());
        }

        menu.addSeparator();
        PopupMenu replaceMenu;
        const auto types = owner.effectChain.getFactory()->getKnownPluginList().getTypes();
        KnownPluginList::addToMenu (replaceMenu, types, KnownPluginList::SortMethod::sortAlphabetically);
        menu.addSubMenu (TRANS ("Replace With..."), replaceMenu);

        menu.addSeparator();
        menu.addItem (deleteId, TRANS ("Delete"), true, false, Image());

        menu.showMenuAsync ({}, [this, numEffects, types] (int result)
        {
            switch (result)
            {
                case selectAllId:       break;
                case deleteId:          deleteEffect(); break;
                case moveUpId:          moveEffect (index - 1); break;
                case moveDownId:        moveEffect (index + 1); break;
                case moveToTopId:       moveEffect (0); break;
                case moveToBottomId:    moveEffect (numEffects - 1); break;

                default:
                {
                    result = KnownPluginList::getIndexChosenByMenu (types, result);
                    if (result < 0)
                        return;

                    closeWindow();

                    setEffect (owner.effectChain.replace (index, types[result].fileOrIdentifier));
                    owner.listbox.updateContent();
                }
                break;
            };
        });                
    }
}

void EffectRowComponent::mouseDoubleClick (const MouseEvent& e)
{
    if (! e.mods.isLeftButtonDown())
        return;

    if (editorWindow != nullptr)
    {
        editorWindow->toFront (true);
        return;
    }

    if (effect == nullptr)
        return;

    if (auto plugin = effect->getPlugin())
    {
        auto* editor = plugin->createEditor();
        if (editor == nullptr)
        {
            editor = new GenericAudioProcessorEditor (*plugin);
            editor->setBounds (0, 0, 800, 800);
        }

        editorWindow = owner.editorWindows.add (new EditorWindow (owner, effect->getName(), effect));
        editorWindow->setIcon (SharedObjects::getWindowIcon());
        editorWindow->setResizable (true, true);
        editorWindow->setContentOwned (editor, true);
        editorWindow->centreWithSize (editor->getWidth(), editor->getHeight());
        editorWindow->setUsingNativeTitleBar (true);
        editorWindow->addToDesktop();
        editorWindow->setVisible (true);
    }
}

//==============================================================================
EffectChainComponent::EffectChainComponent (SharedObjects& so, EffectProcessorChain& epc) :
    sharedObjects (so),
    effectChain (epc)
{
    contentUpdateTimer.callback = [this]()
    {
        listbox.updateContent();
        contentUpdateTimer.stopTimer();
    };

    listbox.setRowHeight (48);
    listbox.setMultipleSelectionEnabled (true);
    listbox.setModel (this);
    addAndMakeVisible (listbox);

    effectChain.addListener (this);
}

EffectChainComponent::~EffectChainComponent()
{
    effectChain.removeListener (this);
}

void EffectChainComponent::deleteEffect (int index, bool shouldUpdateContent)
{
    if (auto effectPtr = effectChain.getEffectProcessor (index))
    {
        for (int f = editorWindows.size(); --f >= 0;)
            if (auto* ew = editorWindows.getUnchecked (f))
                if (ew->getEffect() == effectPtr)
                    ew->closeButtonPressed();

        if (listbox.isRowSelected (index))
            listbox.flipRowSelection (index);

        effectChain.remove (index);
    }
    else
    {
        jassertfalse;
    }

    if (shouldUpdateContent)
        listbox.updateContent();

    repaint();
}

void EffectChainComponent::audioProcessorParameterChanged (AudioProcessor*, int, float)
{
    contentUpdateTimer.startTimer (1000);
}

void EffectChainComponent::audioProcessorChanged (AudioProcessor*, const ChangeDetails&)
{
    contentUpdateTimer.startTimer (1000);
}

Component* EffectChainComponent::refreshComponentForRow (int row, bool, Component* comp)
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
        erc.reset (new EffectRowComponent (*this));

    erc->setIndex (row)
        .setEffect (effectProc);

    return erc.release();
}

void EffectChainComponent::deleteKeyPressed (int)
{
    const auto sr = listbox.getSelectedRows();

    for (int i = sr.size(); --i >= 0;)
        deleteEffect (sr[i], false);

    listbox.updateContent();
}

//==============================================================================
EffectChainDemo::EffectChainDemo (SharedObjects& sharedObjs) :
    DemoBase (sharedObjs, NEEDS_TRANS ("Effect Chain")),
    effectChainComponent (sharedObjs, *effectChain)
{
    clear();

    sharedObjects.audioDeviceManager.addChangeListener (this);
    sharedObjects.audioDeviceManager.addAudioCallback (&audioProcessorPlayer);
    audioProcessorPlayer.setProcessor (&graph);

    play.onClick = [this]()
    {
        if (! play.isEnabled())
            rewindAndStop();
        else if (transport->isPlaying())
            stop();
        else
            transport->play();
    };

    addEffect.onClick = [this]()
    {
        const auto types = knownPluginList.getTypes();
        PopupMenu menu;

        KnownPluginList::addToMenu (menu, types, KnownPluginList::SortMethod::sortAlphabetically);

        menu.showMenuAsync ({}, [this, types] (int result)
        {
            result = KnownPluginList::getIndexChosenByMenu (types, result);

            if (result >= 0)
                effectChain->add (types[result].fileOrIdentifier);
        });
    };

    applyEffects.onStateChange = [this]()
    {
        const auto toggled = applyEffects.getToggleState();

        if (effectChain != nullptr)
            effectChain->setBypassed (! toggled);

        if (toggled)
            applyEffects.setButtonText (TRANS ("FX Active"));
        else
            applyEffects.setButtonText (TRANS ("FX Bypassed"));
    };

    applyEffects.setClickingTogglesState (true);
    applyEffects.setToggleState (! effectChain->isBypassed(), sendNotificationSync);

    loop.onClick        = [this]() { updateLoopState(); };
    load.onClick        = [this]() { loadAudioFile(); };
    goToStart.onClick   = [this]() { rewindAndStop(); };

    loop.setClickingTogglesState (true);
    loop.setToggleState (true, sendNotification);
    updateLoopState();

    play.addShortcut (KeyPress (KeyPress::spaceKey));
    play.addShortcut (KeyPress (KeyPress::playKey));
    loop.addShortcut (KeyPress::createFromDescription ("L"));
    load.addShortcut (KeyPress::createFromDescription ("CMD + O"));
    goToStart.addShortcut (KeyPress (KeyPress::homeKey));
    goToStart.addShortcut (KeyPress (KeyPress::rewindKey));
    addEffect.addShortcut (KeyPress (KeyPress::numberPadAdd));

    auto addLabel = [&] (Label& label)
    {
        label.setJustificationType (Justification::centredLeft);
        label.setColour (Label::backgroundColourId, Colours::transparentBlack);
        label.setColour (Label::textColourId, Colours::black);
        addAndMakeVisible (label);
    };

    addLabel (filePath);
    addLabel (timeDisplay);

    thumbnailRepainter.callback = [this]()
    {
        repaint();

        if (audioThumbnail == nullptr || audioThumbnail->isFullyLoaded())
            thumbnailRepainter.stopTimer();
    };

    playbackRepainter.callback = [this]()
    {
        timeKeeper.setTime (transport->getCurrentTimeSeconds());
        timeDisplay.setText (timeKeeper.toString(), sendNotification);

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

    updateFromAudioDeviceManager();
    reconnect();

    addAndMakeVisible (play);
    addAndMakeVisible (loop);
    addAndMakeVisible (goToStart);
    addAndMakeVisible (load);
    addAndMakeVisible (addEffect);
    addAndMakeVisible (applyEffects);
    addAndMakeVisible (filePath);
    addAndMakeVisible (effectChainComponent);
}

EffectChainDemo::~EffectChainDemo()
{
    sharedObjects.audioDeviceManager.removeChangeListener (this);
    sharedObjects.audioDeviceManager.removeAudioCallback (&audioProcessorPlayer);
    audioProcessorPlayer.setProcessor (nullptr);
}

void EffectChainDemo::changeListenerCallback (ChangeBroadcaster*)
{
    updateFromAudioDeviceManager();
    reconnect();
}

void EffectChainDemo::mouseDown (const MouseEvent& e)
{
    wasPlaying = transport->isPlaying();
    stop();
    movePlayhead (e);
}

void EffectChainDemo::mouseDrag (const MouseEvent& e)
{
    if (audioThumbnailArea.isEmpty() || audioThumbnail == nullptr)
        return;

    isDraggingPlayhead = true;
    movePlayhead (e);
}

void EffectChainDemo::mouseUp (const MouseEvent& e)
{
    if (wasPlaying)
        transport->play();

    wasPlaying = false;
    isDraggingPlayhead = false;
    movePlayhead (e);
}

void EffectChainDemo::resized()
{
    constexpr auto margin = 4;

    auto b = getLocalBounds().reduced (margin);

    {
        auto bottom = b.removeFromBottom (b.getHeight() / 2);
        bottom.removeFromTop (margin);

        effectChainComponent.setBounds (bottom);
    }

    {
        Button* buttons[] =
        {
            &load,
            &loop,
            &play,
            &goToStart,
            &addEffect,
            &applyEffects
        };

        auto topArea = b.removeFromTop (32);
        topArea.removeFromLeft (margin);
        topArea.removeFromRight (margin);

        const auto w = topArea.getWidth() / (int) std::size (buttons);

        for (auto* button : buttons)
            button->setBounds (topArea.removeFromLeft (w));
    }

    b.removeFromTop (margin);

    {
        auto row = b.removeFromTop (32);
        filePath.setBounds (row.removeFromLeft (row.getWidth() / 2));
        timeDisplay.setBounds (row);
    }

    b.removeFromTop (margin);
    audioThumbnailArea = b;
}

void EffectChainDemo::paint (Graphics& g)
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

void EffectChainDemo::updateFromAudioDeviceManager()
{
    timeKeeper.setSamplingRate (sharedObjects.audioDeviceManager);
}

void EffectChainDemo::updateLoopState()
{
    if (transport != nullptr)
        transport->setLooping (loop.getToggleState());
}

void EffectChainDemo::reconnect()
{
    const auto setup = sharedObjects.audioDeviceManager.getAudioDeviceSetup();

    if (setup.inputChannels.toInteger() <= 0
        || setup.outputChannels.toInteger() <= 0)
    {
        return;
    }

    auto nodes = graph.getNodes();

    for (const auto& node : graph.getNodes())
        graph.disconnectNode (node->nodeID);

    auto connect = [&] (Node& source, Node& dest, bool isMidi = false)
    {
        auto addConnection = [&] (int channelIndex)
        {
            const AudioProcessorGraph::NodeAndChannel sourceNAC =
            {
                source->nodeID,
                channelIndex
            };

            const AudioProcessorGraph::NodeAndChannel destNAC =
            {
                dest->nodeID,
                channelIndex
            };

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

void EffectChainDemo::stop()
{
    transport->stop();
    repaint();
}

void EffectChainDemo::rewindAndStop()
{
    transport->stop();
    transport->setCurrentTime (0.0);
    repaint();
}

void EffectChainDemo::clear()
{
    rewindAndStop();
    transport->clear();
    readerSource.reset();
    play.setEnabled (false);
    goToStart.setEnabled (false);
}

void EffectChainDemo::movePlayhead (const MouseEvent& e)
{
    if (audioThumbnailArea.isEmpty() || audioThumbnail == nullptr)
        return;

    const auto mousePos = audioThumbnailArea.getConstrainedPoint (e.getMouseDownPosition());
    const auto timeSeconds = jmap (mousePos.toDouble().x,
                                   (double) audioThumbnailArea.getX(),
                                   (double) audioThumbnailArea.getWidth(),
                                   0.0, audioThumbnail->getTotalLength());

    transport->setCurrentTime (timeSeconds);
    repaint();
}

void EffectChainDemo::setFile (const File& file, AudioFormatManager* audioFormatManager)
{
    clear();

    if (! file.existsAsFile() || audioFormatManager == nullptr)
        return;

    if (auto* reader = audioFormatManager->createReaderFor (file))
    {
        readerSource.reset (new AudioFormatReaderSource (reader, true));

        transport->setSource (readerSource.get(), false, 0, nullptr,
                              sharedObjects.audioDeviceManager.getAudioDeviceSetup().sampleRate);
        play.setEnabled (true);
        goToStart.setEnabled (true);
    }
}

void EffectChainDemo::loadAudioFile()
{
    if (chooser != nullptr)
        return;

    stop();

    chooser.reset (new FileChooser (TRANS ("Load an audio file to process."), {},
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
