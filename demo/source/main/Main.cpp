/** */
class MainWindow final : public DocumentWindow
{
public:
    MainWindow (const String& name) :
        DocumentWindow (name, Colours::black, DocumentWindow::allButtons),
        customLookAndFeel (sharedObjects)
    {
        Desktop::getInstance().setDefaultLookAndFeel (&customLookAndFeel);

        splash = new SquarePineDemoSplashScreen();

        splash->onComplete = [ptr = SafePointer (this)]()
        {
            MessageManager::callAsync ([ptr]()
            {
                SQUAREPINE_CRASH_TRACER
                if (ptr != nullptr)
                {
                    ptr->init();
                    Process::makeForegroundProcess();
                }
            });
        };

        splash->run();
    }

    ~MainWindow() override
    {
        Desktop::getInstance().setDefaultLookAndFeel (nullptr);
    }

    //==============================================================================
    /** @internal */
    void closeButtonPressed() override
    {
        JUCEApplication::getInstance()->systemRequestedQuit();
    }

    /** @internal */
    void parentSizeChanged() override
    {
        for (auto& c : getChildren())
            c->resized();
    }

private:
    //==============================================================================
    SharedObjects sharedObjects;
    SharedResourcePointer<sp::GoogleAnalyticsReporter> googleAnalyticsReporter;
    DemoLookAndFeel customLookAndFeel;

   #if SQUAREPINE_IS_DESKTOP
    std::unique_ptr<SystemTrayIconComponent> trayIconComponent;
   #endif

    std::unique_ptr<MainComponent> mainComponent;

    SafePointer<SquarePineDemoSplashScreen> splash;

    //==============================================================================
    void init()
    {
        googleAnalyticsReporter->startSession();

        setUsingNativeTitleBar (true);
        setOpaque (true);

       #if SQUAREPINE_IS_DESKTOP
        trayIconComponent.reset (new TrayIconComponent (sharedObjects.commandManager));
       #endif

        mainComponent = std::make_unique<MainComponent> (sharedObjects);
        setContentNonOwned (mainComponent.get(), true);

       #if SQUAREPINE_IS_MOBILE
        setFullScreen (true);
       #else
        setResizable (true, true);
        centreWithSize (getWidth(), getHeight());
       #endif

       #if SQUAREPINE_USE_ICUESDK
        corsair::globalSession->logInfo();
       #endif

        setVisible (true);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};

//==============================================================================
class SquarePineDemoApplication final : public sp::SimpleApplication
{
public:
    /** */
    SquarePineDemoApplication() = default;

    //==============================================================================
    /** @internal */
    String getAppName() const override      { return ProjectInfo::projectName; }
    /** @internal */
    String getAppVersion() const override   { return SQUAREPINE_VERSION_STRING; }

    //==============================================================================
    /** @internal */
    std::unique_ptr<DocumentWindow> createWindow() const override
    {
        return std::make_unique<MainWindow> (makeMainWindowTitle (getAppName(), getAppVersion()));
    }

    String getLoggerFilePath() const override
    {
        return GlobalPaths::getLogFile().getParentDirectory().getFullPathName();
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineDemoApplication)
};

//==============================================================================
START_JUCE_APPLICATION (SquarePineDemoApplication)
