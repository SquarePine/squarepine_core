#if SQUAREPINE_IS_DESKTOP

/** This is just a generic and simplistic tray icon component,
    instantiated on desktop systems only (which is the only place it's supported by JUCE).

    @see MainWindow, juce::SystemTrayIconComponent
*/
class TrayIconComponent final : public SystemTrayIconComponent
{
public:
    TrayIconComponent (ApplicationCommandManager& cm) :
        commandManager (cm)
    {
        setAccessible (false);

        const auto image = ImageCache::getFromMemory (BinaryData::SquarePine_Logo_png, BinaryData::SquarePine_Logo_pngSize);
        setIconImage (image, image);
    }

    //==============================================================================
    void mouseDown (const MouseEvent& e) override
    {
        lastEventMods = e.mods;
    }

    void mouseUp (const MouseEvent&) override
    {
        auto& desktop = Desktop::getInstance();

        for (int i = desktop.getNumComponents(); --i >= 0;)
            desktop.getComponent (i)->toFront (true);

        // NB: by this point, the mouse buttons are... well, up/inactive.
        if (lastEventMods.isPopupMenu()
            && commandManager.getNumCommands() > 0)
        {
            PopupMenu pm;

            pm.addCommandItem (&commandManager, WorkstationIds::preferences);
            pm.addSeparator();
            pm.addCommandItem (&commandManager, StandardApplicationCommandIDs::quit);

            pm.showMenuAsync ({});
        }

        lastEventMods = {};
    }

private:
    //==============================================================================
    ApplicationCommandManager& commandManager;
    ModifierKeys lastEventMods;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrayIconComponent)
};

#endif // SQUAREPINE_IS_DESKTOP

//==============================================================================
class MainWindow final : public DocumentWindow
{
public:
    MainWindow (SharedObjects& sharedObjects, const String& name) :
        DocumentWindow (name, Colours::black, DocumentWindow::allButtons),
        customLookAndFeel (sharedObjects)
    {
        setAccessible (false);
        setUsingNativeTitleBar (true);
        setOpaque (true);

        Desktop::getInstance().setDefaultLookAndFeel (&customLookAndFeel);

       #if SQUAREPINE_IS_DESKTOP
        trayIconComponent.reset (new TrayIconComponent (sharedObjects.commandManager));
       #endif

        setContentOwned (new MainComponent (sharedObjects), true);

       #if SQUAREPINE_IS_MOBILE
        setFullScreen (true);
       #else
        setResizable (true, true);
        centreWithSize (getWidth(), getHeight());
       #endif

        setVisible (true);
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

private:
    //==============================================================================
    DemoLookAndFeel customLookAndFeel;
    TooltipWindow tooltipWindow;

    std::unique_ptr<SystemTrayIconComponent> trayIconComponent;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};

//==============================================================================
class SquarePineDemoApplication final : public JUCEApplication
{
public:
    /** */
    SquarePineDemoApplication()                     { SystemStats::setApplicationCrashHandler (CrashStackTracer::dump); }

    //==============================================================================
    /** @internal */
    const String getApplicationName() override      { return ProjectInfo::projectName; }
    /** @internal */
    const String getApplicationVersion() override   { return ProjectInfo::versionString; }
    /** @internal */
    bool moreThanOneInstanceAllowed() override      { return false; }
    /** @internal */
    void systemRequestedQuit() override             { quit(); }

    //==============================================================================
    /** @internal */
    void initialise (const String& commandLineArgs) override
    {
        SQUAREPINE_CRASH_TRACER

        if (runFromCommandLine (commandLineArgs))
            quit();

        String windowName;
        windowName
            << ProjectInfo::companyName
            << " - " << getApplicationName()
            << " - v"
            << getApplicationVersion();

       #if JUCE_DEBUG
        windowName << " [DEBUG]";
       #endif

        sharedObjects.reset (new SharedObjects());
        googleAnalyticsReporter->startSession();
        mainWindow.reset (new MainWindow (*sharedObjects.get(), windowName));
    }

    void anotherInstanceStarted (const String& commandLineArgs) override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Started new instance of application...");

        runFromCommandLine (commandLineArgs);
        quit();
    }

    void suspended() override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Application was suspended.");
    }

    void resumed() override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Application was resumed.");
    }

    void unhandledException (const std::exception* e,
                             const String& sourceFilename,
                             int lineNumber) override
    {
        SQUAREPINE_CRASH_TRACER

        String message;
        message
            << "ERROR --- received unhandled std::exception!" << newLine
            << "Line Number: " << lineNumber << newLine
            << "Source Filename: " << sourceFilename << newLine;

        if (e != nullptr)
            message << "What: " << e->what() << newLine;

        Logger::writeToLog (message);
        jassertfalse;
    }

    void memoryWarningReceived() override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Received memory warning from OS!");
        jassertfalse;
    }

    bool backButtonPressed() override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Back button was pressed.");
        return false;
    }

    void shutdown() override
    {
        SQUAREPINE_CRASH_TRACER

        Logger::writeToLog ("Shutdown called.");
        googleAnalyticsReporter->endSession();
        mainWindow = nullptr;
        sharedObjects = nullptr;
    }

private:
    //==============================================================================
    std::unique_ptr<SharedObjects> sharedObjects;
    std::unique_ptr<MainWindow> mainWindow;
    SharedResourcePointer<sp::GoogleAnalyticsReporter> googleAnalyticsReporter;

    //==============================================================================
    bool runFromCommandLine (const String& commandLineArgs)
    {
        const auto args = commandLineArgs.trim();

        if (args.isNotEmpty())
            Logger::writeToLog (String ("Unknown command line args given: ") + newLine + args);

        return false;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineDemoApplication)
};

//==============================================================================
START_JUCE_APPLICATION (SquarePineDemoApplication)
