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

MainComponent::MainComponent (SharedObjects& sharedObjs) :
    tabbedComponent (TabbedButtonBar::TabsAtTop)
{
    setOpaque (true);

    auto addTab = [&] (DemoBase* comp)
    {
        // tabbedComponent.addTab (TRANS (comp->getName()), Colours::grey, comp, true);

        demos.add (comp);
    };

    addTab (new EaseListComponent (sharedObjs));
    addTab (new EffectChainDemo (sharedObjs));
    addTab (new ImageDemo (sharedObjs));
    addTab (new CodeEditorDemo (sharedObjs));
    addTab (new MediaDeviceListerDemo (sharedObjs));
    addTab (new AnimationDemo (sharedObjs));
    addTab (new ParticleSystemDemo (sharedObjs));

   #if SQUAREPINE_USE_ICUESDK
    addTab (new iCUESDKDemo (sharedObjs));
   #endif

   #if SQUAREPINE_USE_WINRTRGB
    addTab (new WinRTRGBDemo (sharedObjs));
   #endif

   #if SP_DEMO_USE_OPENGL
    // Need to call this later on - once JUCE, the GL content, and the OS decide it's cool to talk to each other.
    MessageManager::callAsync ([this, ptr = SafePointer (this)]()
    {
        SQUAREPINE_CRASH_TRACER
        if (ptr != nullptr)
        {
            rendererConfigurator.configureWithOpenGLIfAvailable (*this);
            resized();
        }
    });

    addTab (new OpenGLDetailsDemo (sharedObjs, rendererConfigurator));
   #endif // SP_DEMO_USE_OPENGL

    addTab (new SettingsComponent (sharedObjs));

    tabbedComponent.setOutline (0);
    tabbedComponent.setIndent (0);

    burgerMenu.setModel (this);
    menuItemSelected (1, 0);
    addAndMakeVisible (burgerMenu);

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
    return { TRANS ("SquarePine Demo"), TRANS ("Demos") };
}

PopupMenu MainComponent::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
    if (topLevelMenuIndex != 1)
        return {};

    PopupMenu pm;

    int index = 1;

    for (auto* demo : demos)
        pm.addItem (index++, TRANS (demo->getName()), true, demo == activeDemo);

    return pm;
}

void MainComponent::menuItemSelected (int menuItemId, int topLevelMenuIndex)
{
    --menuItemId;

    if (auto* demoToSelect = demos[menuItemId])
    {
        removeChildComponent (activeDemo);

        activeDemo = demoToSelect;

        addAndMakeVisible (activeDemo);
        resized();
    }
}

void MainComponent::menuBarActivated (bool isActive)
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
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

    b = b.reduced (4);

    burgerMenu.setBounds (b);

    if (activeDemo != nullptr)
        activeDemo->setBounds (b);
}

void MainComponent::languageChanged (const IETFLanguageFile&)
{
    SQUAREPINE_CRASH_TRACER

    for (int i = demos.size(); --i >= 0;)
        tabbedComponent.setTabName (i, TRANS (demos.getUnchecked (i)->getUntranslatedName()));
}
