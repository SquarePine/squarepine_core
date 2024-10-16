#if SQUAREPINE_IS_MOBILE

inline Rectangle<int> getBoundsAccountingForKeyboard()
{
    if (auto* display = Desktop::getInstance().getDisplays().getPrimaryDisplay())
    {
        auto bounds = display->userArea;

        // display->safeAreaInsets.subtractFrom (bounds);
        // display->keyboardInsets.subtractFrom (bounds);

        return bounds;
    }

    return {};
}

#endif

MainComponent::MainComponent (SharedObjects& sharedObjs)
{
    SQUAREPINE_CRASH_TRACER

    setOpaque (true);

    MessageManager::callAsync ([this, ptr = SafePointer (this)]()
    {
        SQUAREPINE_CRASH_TRACER
        if (ptr != nullptr)
        {
           #if SP_DEMO_USE_OPENGL
            // Need to call this later on - once JUCE, the GL content, and the OS decide it's cool to talk to each other.
            rendererConfigurator.configureWithOpenGLIfAvailable (*this);
           #endif

            updateTranslations();
            resized();
        }
    });

    auto addDemo = [&] (DemoBase* comp)
    {
        demos.add (comp);
    };

    addDemo (new EaseListComponent (sharedObjs));
    addDemo (new EffectChainDemo (sharedObjs));
    addDemo (new ImageDemo (sharedObjs));
    addDemo (new CodeEditorDemo (sharedObjs));
    addDemo (new MediaDeviceListerDemo (sharedObjs));
    addDemo (new AnimationDemo (sharedObjs));
    addDemo (new ParticleSystemDemo (sharedObjs));

   #if SQUAREPINE_USE_ICUESDK
    addDemo (new iCUESDKDemo (sharedObjs));
   #endif

   #if SQUAREPINE_USE_WINRTRGB
    addDemo (new WinRTRGBDemo (sharedObjs));
   #endif

   #if SP_DEMO_USE_OPENGL
    addDemo (new OpenGLDetailsDemo (sharedObjs, rendererConfigurator));
   #endif

    addDemo (new SettingsComponent (sharedObjs));

    popupButton.onClick = [this, ptr = SafePointer (this)]()
    {
        if (ptr == nullptr)
            return;

        auto menu = getMenuForIndex (0, {});
        menu.showMenuAsync (PopupMenu::Options(),
        [this, ptr] (int index)
        {
            if (ptr != nullptr)
                menuItemSelected (index, 0);
        });
    };

    addAndMakeVisible (popupButton);
    menuItemSelected (7, 0);

   #if SQUAREPINE_IS_DESKTOP
    setSize (1024, 768);
   #else
    setBounds (getBoundsAccountingForKeyboard());
   #endif
}

MainComponent::~MainComponent()
{
}

//==============================================================================
StringArray MainComponent::getMenuBarNames()
{
    return { TRANS ("Demos") };
}

PopupMenu MainComponent::getMenuForIndex (int topLevelMenuIndex, const String&)
{
    SQUAREPINE_CRASH_TRACER

    if (topLevelMenuIndex != 0)
        return {};

    PopupMenu pm;

    int index = 1;

    for (auto* demo : demos)
        pm.addItem (index++, TRANS (demo->getName()), true, demo == activeDemo);

    return pm;
}

void MainComponent::menuItemSelected (int menuItemId, int topLevelMenuIndex)
{
    SQUAREPINE_CRASH_TRACER

    if (topLevelMenuIndex != 0)
        return;

    --menuItemId;

    if (auto* demoToSelect = demos[menuItemId])
    {
        removeChildComponent (activeDemo);

        activeDemo = demoToSelect;

        addAndMakeVisible (activeDemo);
        resized();
    }
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    const auto background = getLookAndFeel().findColour (ResizableWindow::backgroundColourId);
    g.fillAll (background);

    g.setColour (background.darker());
    g.fillRect (barArea);
}

void MainComponent::resized()
{
    SQUAREPINE_CRASH_TRACER

    auto b = getLocalBounds();

   #if SQUAREPINE_IS_MOBILE
    b = getBoundsAccountingForKeyboard();
    if (b.isEmpty())
        b = getLocalBounds();
   #endif

    b = b.reduced (DemoBase::marginPx);

    {
        barArea = b.removeFromLeft (DemoBase::barSizePx * 2)
                   .reduced (DemoBase::marginPx);

        auto tempBar = barArea;
        popupButton.setBounds (tempBar.removeFromTop (tempBar.getWidth()));
    }

    if (activeDemo != nullptr)
        activeDemo->setBounds (b);
}

void MainComponent::updateTranslations()
{
    SQUAREPINE_CRASH_TRACER

    popupButton.setButtonText (TRANS ("Select Demo"));
}
