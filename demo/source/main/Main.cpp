class SquarePineDemoApplication final : public JUCEApplication
{
public:
    SquarePineDemoApplication()                             { }
    const String getApplicationName() override              { return ProjectInfo::projectName; }
    const String getApplicationVersion() override           { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override              { return false; }
    void systemRequestedQuit() override                     { quit(); }
    void anotherInstanceStarted (const String&) override    { }

    void initialise (const String&) override
    {
        String windowName;
        windowName
            << getApplicationName()
            << " - v"
            << getApplicationVersion();

       #if JUCE_DEBUG
        windowName << " [DEBUG]";
       #endif

        mainWindow.reset (new MainWindow (windowName));

        googleAnalyticsReporter->startSession();
    }

    void shutdown() override
    {
        googleAnalyticsReporter->endSession();
        mainWindow = nullptr;
    }

private:
    //==============================================================================
    class MainWindow final : public DocumentWindow
    {
    public:
        MainWindow (const String& name) :
            DocumentWindow (name, Desktop::getInstance().getDefaultLookAndFeel()
                                    .findColour (ResizableWindow::backgroundColourId),
                            DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (true, true);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

    //==============================================================================
    std::unique_ptr<MainWindow> mainWindow;
    SharedResourcePointer<sp::GoogleAnalyticsReporter> googleAnalyticsReporter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquarePineDemoApplication)
};

//==============================================================================
START_JUCE_APPLICATION (SquarePineDemoApplication)
