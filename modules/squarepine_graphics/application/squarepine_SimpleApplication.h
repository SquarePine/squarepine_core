/** A simple application class wrapping the general set of functionality
    satisfying most applications' needs.

    I found it unusually tiring to repeatedly set up the same various odds and ends within
    an app when starting a new project, from setting up a decent file logger
    to having a window title that makes sense and differentiates between release and debug.

    Another main benefit to this super class is that this will log any function
    you haven't filled in yourself, like unhandledException(), suspended() and resumed(), etc.
    This way you're aware that these functions are getting called.
    This is especially useful on mobile where debugging can be frustrating.
*/
class SimpleApplication : public JUCEApplication
{
public:
    /** Constructor. */
    SimpleApplication();

    /** Destructor. */
    ~SimpleApplication() override;

    //==============================================================================
    /** @returns the application's name.
        Unlike JUCEApplication's idiotic getApplicationName which isn't method-const,
        this method allows you to be correct-er.
    */
    virtual String getAppName() const = 0;

    /** @returns the application's version number.
        Unlike JUCEApplication's idiotic getApplicationVersion which isn't method-const,
        this method allows you to be correct-er.
    */
    virtual String getAppVersion() const = 0;

    //==============================================================================
    /** @returns the last known command line arguments that were passed to this application.
        If another instance was started (assuming that was allowed),
        these arguments will be overridden by that.

        @see juce::ApplicationBase::getCommandLineParameterArray,
             juce::ApplicationBase::getCommandLineParameters,
    */
    const String& getRawCommandLineArguments() const noexcept { return commandLineArguments; }

    /** When returning from some modal components this will become the command target:
        this doesn't handle any commands so ensure the command chain is correctly passed on
        to the applications main ApplicationCommandTarget.
    */
    void setMainCommandTarget (ApplicationCommandTarget*);

    //==============================================================================
    /** @internal */
    const String getApplicationName() final { return getAppName(); }
    /** @internal */
    const String getApplicationVersion() final { return getAppVersion(); }
    /** @internal */
    bool moreThanOneInstanceAllowed() override { return false; }
    /** @internal */
    void initialise (const String&) override;
    /** @internal */
    void shutdown() override;
    /** @internal */
    void anotherInstanceStarted (const String&) override;
    /** @internal */
    ApplicationCommandTarget* getNextCommandTarget() override { return mainCommandTarget; }
    /** @internal */
    void suspended() override;
    /** @internal */
    void resumed() override;
    /** @internal */
    void unhandledException (const std::exception*, const String&, int) override;
    /** @internal */
    void memoryWarningReceived() override;
    /** @internal */
    bool backButtonPressed() override;

protected:
    //==============================================================================
    /** */
    virtual std::unique_ptr<DocumentWindow> createWindow() const = 0;

    /** */
    virtual bool handleInternalCommandLineOperations (const String& commandLine);

    /** */
    virtual String getLoggerFilePath() const;

    //==============================================================================
    /** @returns true if anything unit test related has been performed. */
    virtual bool handleUnitTests (const String& commandLine);

private:
    //==============================================================================
    /** */
    class InternalUnitTestRunner final : public UnitTestRunner
    {
    public:
        InternalUnitTestRunner() = default;
        void setAbortingTests (bool shouldAbortTests)   { abortTests = shouldAbortTests; }
        bool shouldAbortTests() override                { return abortTests; }

       #if JUCE_WINDOWS
        /** On Windows we need to override the default logMessage so that it prints to both the debugger and command-line console. */
        void logMessage (const String& message) override
        {
            DBG (message);
            if (message.containsIgnoreCase ("error"))
                std::cerr << message << newLine;
            else
                std::cout << message << newLine;
        }
       #endif

    private:
        std::atomic<bool> abortTests { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InternalUnitTestRunner)
    };

    std::unique_ptr<FileLogger> logger;
    InternalUnitTestRunner unitTestRunner;
    ApplicationCommandTarget* mainCommandTarget = nullptr;
    String commandLineArguments;
   #if SQUAREPINE_IS_DESKTOP
    TooltipWindow tooltipWindow;
   #endif
    std::unique_ptr<DocumentWindow> mainWindow;

    //==============================================================================
    void enableLogging();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleApplication)
};
