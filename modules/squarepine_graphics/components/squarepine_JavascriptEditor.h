#if JUCE_MODULE_AVAILABLE_squarepine_duktape

class JavascriptEditor final : public Component,
                               private CodeDocument::Listener,
                               private Timer
{
public:
    JavascriptEditor()
    {
        setOpaque (true);

        engine.deregisterNativeObject ("console");
        engine.registerNativeObject ("console", new LoggerClass (*this));

        editor.reset (new CodeEditorComponent (codeDocument, &tokeniser));

        editor->setColourScheme (sp::JavascriptTokeniser::getDefaultEditorColourScheme());
        editor->setTabSize (4, true);
        editor->setFont ({ Font::getDefaultMonospacedFontName(), 18.0f, Font::plain });

        if (Font::findAllTypefaceNames().contains ("consolas", true))
            editor->setFont ({ "consolas", 18.0f, Font::plain });

        editor->loadContent (
            "/*\n"
            "    Javascript!\n"
            "*/\n"
            "\n"
            "console.log (\"Hello World in Javascript!\");\n"
            "console.log (\"\");\n"
            "\n"
            "function factorial (n)\n"
            "{\n"
            "    var total = 1;\n\n"
            "    while (n > 0)\n"
            "        total = total * n--;\n\n"
            "    return total;\n"
            "}\n"
            "\n"
            "for (var i = 1; i < 10; ++i)\n"
            "    console.log (\"Factorial of \" + i \n"
            "                   + \" = \" + factorial (i));\n");

        startTimer (1000);

        outputDisplay.setMultiLine (true);
        outputDisplay.setScrollbarsShown (true);
        outputDisplay.setReadOnly (true);
        outputDisplay.setCaretVisible (false);
        outputDisplay.setFont ({ Font::getDefaultMonospacedFontName(), 14.0f, Font::plain });

        codeDocument.addListener (this);

        addAndMakeVisible (outputDisplay);
        addAndMakeVisible (editor.get());
        setSize (800, 800);
    }

    //==============================================================================
    void consoleOutput (const String& message)
    {
        outputDisplay.moveCaretToEnd();
        outputDisplay.insertTextAtCaret (message + newLine);
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
    }

    void resized() override
    {
        auto r = getLocalBounds().reduced (8);

        editor->setBounds (r.removeFromTop (proportionOfHeight (0.6f)));
        outputDisplay.setBounds (r.withTrimmedTop (8));
    }

    void lookAndFeelChanged() override
    {
        outputDisplay.applyFontToAllText (outputDisplay.getFont());
    }

    //==============================================================================
    sp::JavascriptEngine engine;

private:
    //==============================================================================
    class LoggerClass final : public DynamicObject
    {
    public:
        LoggerClass (JavascriptEditor& p) :
            parent (p)
        {
            setMethod ("log", [&] (const var::NativeFunctionArgs& a)
            {
                MemoryOutputStream mo (1024);

                for (int i = 0; i < a.numArguments; ++i)
                    mo << a.arguments[i].toString() << newLine;

                parent.outputDisplay.insertTextAtCaret (mo.toString());
                return var::undefined();
            });
        }

        JavascriptEditor& parent;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoggerClass)
    };

    friend class LoggerClass;

    //==============================================================================
    sp::JavascriptTokeniser tokeniser;
    CodeDocument codeDocument;
    std::unique_ptr<CodeEditorComponent> editor;
    TextEditor outputDisplay;

    //==============================================================================
    void runScript()
    {
        outputDisplay.clear();

        MillisecondStopWatch stopWatch;

        stopWatch.start();
        const auto result = engine.execute (codeDocument.getAllContent());
        stopWatch.stop();

        if (result.failed())
            outputDisplay.setText (result.getErrorMessage());

        outputDisplay.insertTextAtCaret (newLine + "(Execution time: " + String (stopWatch.getDelta(), 2) + " milliseconds)");
    }

    void codeDocumentTextInserted (const String&, int) override { startTimer (1500); }
    void codeDocumentTextDeleted (int, int) override            { startTimer (1500); }

    void timerCallback() override
    {
        stopTimer();
        runScript();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JavascriptEditor)
};

#endif // JUCE_MODULE_AVAILABLE_squarepine_duktape
