/** */
class UnitTestsDemo final : public DemoBase
{
public:
    /** */
    UnitTestsDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Unit Tests"))
    {
        addAndMakeVisible (startTestButton);
        startTestButton.onClick = [this] { start(); };

        addAndMakeVisible (testResultsBox);
        testResultsBox.setMultiLine (true);
        testResultsBox.setFont (FontOptions (Font::getDefaultMonospacedFontName(), 16.0f, Font::plain));

        addAndMakeVisible (categoriesBox);
        categoriesBox.addItem ("All Tests", 1);

        SquarePineCoreUnitTestGatherer().appendUnitTests (allTests);
        SquarePineCryptographyUnitTestGatherer().appendUnitTests (allTests);

        auto categories = UnitTest::getAllCategories();
        categories.sort (true);

        categoriesBox.addItemList (categories, 2);
        categoriesBox.setSelectedId (1);
    }

    //==============================================================================
    void start()
    {
        startTest (categoriesBox.getText());
    }

    void startTest (const String& category)
    {
        testResultsBox.clear();
        startTestButton.setEnabled (false);

        currentTestThread.reset (new TestRunnerThread (*this, category));
        currentTestThread->startThread();
    }

    void stopTest()
    {
        if (currentTestThread.get() != nullptr)
        {
            currentTestThread->stopThread (15000);
            currentTestThread.reset();
        }
    }

    void logMessage (const String& message)
    {
        testResultsBox.moveCaretToEnd();
        testResultsBox.insertTextAtCaret (message + newLine);
        testResultsBox.moveCaretToEnd();
    }

    void testFinished()
    {
        stopTest();
        startTestButton.setEnabled (true);
        logMessage (newLine + "*** Tests finished ***");
    }

    //==============================================================================
    void resized() override
    {
        auto bounds = getLocalBounds().reduced (6);

        auto topSlice = bounds.removeFromTop (25);
        startTestButton.setBounds (topSlice.removeFromLeft (200));
        topSlice.removeFromLeft (10);
        categoriesBox.setBounds (topSlice.removeFromLeft (250));

        bounds.removeFromTop (5);
        testResultsBox.setBounds (bounds);
    }

    void lookAndFeelChanged() override
    {
        testResultsBox.applyFontToAllText (testResultsBox.getFont());
    }

private:
    //==============================================================================
    class TestRunnerThread final : public Thread,
                                   private Timer
    {
    public:
        TestRunnerThread (UnitTestsDemo& utd, const String& ctg) :
            Thread ("Unit Tests"),
            owner (utd),
            category (ctg)
        {
        }

        void run() override
        {
            CustomTestRunner runner (*this);

            if (category == "All Tests")
                runner.runAllTests();
            else
                runner.runTestsInCategory (category);

            startTimerHz (1);
        }

        void logMessage (const String& message)
        {
            MessageManager::callAsync ([message, safeOwner = WeakReference<UnitTestsDemo> (&owner)]
            {
                if (auto* o = safeOwner.get())
                    o->logMessage (message);
            });
        }

        void timerCallback() override
        {
            if (! isThreadRunning())
                owner.testFinished(); // inform the demo page when done, so it can delete this thread.
        }

    private:
        //==============================================================================
        // This subclass of UnitTestRunner is used to redirect the test output to our
        // TextBox, and to interrupt the running tests when our thread is asked to stop.
        class CustomTestRunner final : public UnitTestRunner
        {
        public:
            CustomTestRunner (TestRunnerThread& trt) : owner (trt) {}

            void logMessage (const String& message) override
            {
                owner.logMessage (message);
            }

            bool shouldAbortTests() override
            {
                return owner.threadShouldExit();
            }

        private:
            TestRunnerThread& owner;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomTestRunner)
        };

        UnitTestsDemo& owner;
        const String category;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestRunnerThread)
    };

    OwnedArray<UnitTest> allTests;
    std::unique_ptr<TestRunnerThread> currentTestThread;

    TextButton startTestButton { "Run Unit Tests..." };
    ComboBox categoriesBox;
    TextEditor testResultsBox;

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (UnitTestsDemo)
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestsDemo)
};
