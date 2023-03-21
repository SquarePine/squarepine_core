inline Rectangle<int> getBoundsAccountingForKeyboard ()
{
    if (auto* display { Desktop::getInstance ().getDisplays ().getPrimaryDisplay () })
    {
        auto bounds { display->userArea };

        // display->safeAreaInsets.subtractFrom (bounds);
        // display->keyboardInsets.subtractFrom (bounds);

        return bounds;
    }

    return {};
}

MainComponent::MainComponent (SharedObjects& sharedObjs) :
    tabbedComponent (TabbedButtonBar::TabsAtTop)
{
    setOpaque (true);

    auto addTab = [&] (DemoBase* comp)
    {
        tabbedComponent.addTab (TRANS (comp->getName()), Colours::grey, comp, true);
    };

    addTab (new EaseListComponent (sharedObjs));
    addTab (new EffectChainDemo (sharedObjs));
    addTab (new ImageDemo (sharedObjs));
    addTab (new CodeEditorDemo (sharedObjs));
    addTab (new MediaDeviceListerDemo (sharedObjs));

   #if SQUAREPINE_USE_CUESDK
    addTab (new CueSDKDemo (sharedObjs));
   #endif

   #if SP_DEMO_USE_OPENGL
    // Need to call this later on - once JUCE, the GL content, and the OS decide it's cool to talk to each other.
    MessageManager::callAsync ([&]()
    {
        rendererConfigurator.configureWithOpenGLIfAvailable (*this);

        if (auto* context = rendererConfigurator.context.get())
            context->executeOnGLThread ([&] (OpenGLContext& c)
            {
                c.setSwapInterval (1);                  // Make sure vsync is active.
                rendererConfigurator.paintCallback();   // Need to force a repaint.
            }, false);
    });

    addTab (new OpenGLDetailsDemo (sharedObjs, rendererConfigurator));
   #endif // SP_DEMO_USE_OPENGL

    tabbedComponent.addTab (TRANS ("Settings"), Colours::grey, new SettingsComponent (sharedObjs), true);

    addAndMakeVisible (tabbedComponent);

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
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto b = getLocalBounds();

   #if SQUAREPINE_IS_MOBILE
    b = getBoundsAccountingForKeyboard();
    if (b.isEmpty())
        b = getLocalBounds();
   #endif

    tabbedComponent.setBounds (b.reduced (4));
}

void MainComponent::languageChanged (const IETFLanguageFile&)
{
    for (int i = tabbedComponent.getNumTabs(); --i >= 0;)
        if (auto* demoComp = dynamic_cast<DemoBase*> (tabbedComponent.getTabContentComponent (i)))
            tabbedComponent.setTabName (i, TRANS (demoComp->getUntranslatedName()));
}
