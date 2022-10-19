/** A simple application wrapping the general set of functionality
    satisfying most applications' needs.
*/
class SimpleApplication : public JUCEApplication
{
public:
    /** Constructor */
    SimpleApplication();

    /** Destructor */
    ~SimpleApplication() override;

    //==============================================================================
    /** */
    const String& getCommandLineArguments() const noexcept { return commandLineArguments; }

    /** When returning from some modal components this will become the command target:
        this doesn't handle any commands so ensure the command chain is correctly passed on
        to the applications main ApplicationCommandTarget.
    */
    void setMainCommandTarget (ApplicationCommandTarget* target);

    //==============================================================================
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
    virtual DocumentWindow* createWindow() const = 0;

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
    class InternalUnitTestRunner : public UnitTestRunner
    {
    public:
        InternalUnitTestRunner()                       { }
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
    TooltipWindow tooltipWindow;
    std::unique_ptr<DocumentWindow> mainWindow;
    InternalUnitTestRunner unitTestRunner;
    ApplicationCommandTarget* mainCommandTarget = nullptr;
    String commandLineArguments;

    //==============================================================================
    /** JUCE's random system isn't great for every occasion,
        so in here we seed the C/C++ random system.
    */
    void initSystemRandom();

    /** Linux is simply bugged for process naming, and JUCE is doing it right.
        This is a fix only necessary in this broken OS series, as far as we know...
    */
    void initOperatingSystemDependantSystems();

    //==============================================================================
    /** */
    void enableLogging();

    /** */
    void disableLogging();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleApplication)
};
