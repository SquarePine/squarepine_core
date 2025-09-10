/** */
class FigmaClientDemo final : public DemoBase,
                              private Timer
{
public:
    /** */
    FigmaClientDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Figma Client")),
        threadPool (sharedObjs.threadPool)
    {
        // Configure input fields
        tokenInput.setMultiLine (false);
        tokenInput.setReturnKeyStartsNewLine (false);
        tokenInput.setTextToShowWhenEmpty (TRANS ("Enter your Figma personal access token..."), Colours::grey);
        tokenInput.setPasswordCharacter ('*');

        fileKeyInput.setMultiLine (false);
        fileKeyInput.setReturnKeyStartsNewLine (false);
        fileKeyInput.setTextToShowWhenEmpty (TRANS ("Enter Figma file key (from URL)..."), Colours::grey);

        // Configure buttons
        loadButton.onClick = [this] { loadFigmaFile(); };
        clearButton.onClick = [this] { clearContent(); };

        // Add components
        addAndMakeVisible (tokenInput);
        addAndMakeVisible (fileKeyInput);
        addAndMakeVisible (loadButton);
        addAndMakeVisible (clearButton);
        addAndMakeVisible (statusLabel);
        addAndMakeVisible (figmaComponent);

        // Set initial state
        statusLabel.setJustificationType (Justification::centred);
        statusLabel.setColour (Label::textColourId, Colours::white);

        figmaComponent.setVisible (false);
    }

    ~FigmaClientDemo() override
    {
        if (loadingJob != nullptr
            && ! threadPool->removeJob (loadingJob.get(), true, 5000))
        {
            jassertfalse;
        }
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        if (! figmaComponent.isVisible())
        {
            g.setColour (Colours::white);
            sp::drawFittedText (g, TRANS ("Enter your Figma token and file key to load a design!"),
                               getLocalBounds(), Justification::centred);
        }
    }

    /** @internal */
    void resized() override
    {
        auto bounds = getLocalBounds().reduced (dims::marginPx);

        // Top controls area
        auto controlsArea = bounds.removeFromTop (120);

        // Token input
        auto tokenArea = controlsArea.removeFromTop (30);
        tokenInput.setBounds (tokenArea);

        // File key input
        auto fileKeyArea = controlsArea.removeFromTop (30);
        fileKeyInput.setBounds (fileKeyArea);

        // Buttons
        auto buttonArea = controlsArea.removeFromTop (30);
        loadButton.setBounds (buttonArea.removeFromLeft (120));
        clearButton.setBounds (buttonArea.removeFromLeft (120));

        // Status label
        auto statusArea = controlsArea.removeFromTop (30);
        statusLabel.setBounds (statusArea);

        // Figma content area
        figmaComponent.setBounds (bounds);
    }

    void updateWithNewTranslations() override
    {
        tokenInput.setTextToShowWhenEmpty (TRANS ("Enter your Figma personal access token..."), Colours::grey);
        fileKeyInput.setTextToShowWhenEmpty (TRANS ("Enter Figma file key (from URL)..."), Colours::grey);
        loadButton.setButtonText (TRANS ("Load File"));
        clearButton.setButtonText (TRANS ("Clear"));
        statusLabel.setText (TRANS ("Ready to load Figma file"), dontSendNotification);
        repaint();
    }

    void timerCallback() override
    {
    }

private:
    //==============================================================================
    class FigmaLoadingJob final : public ThreadPoolJob
    {
    public:
        FigmaLoadingJob (const String& token, const String& fileKey) :
            ThreadPoolJob ("FigmaLoadingJob"),
            personalAccessToken (token),
            figmaFileKey (fileKey)
        {
        }

        std::function<void(var, String)> onComplete;

        JobStatus runJob() override
        {
            FigmaClient client (personalAccessToken);
            String errorMessage;

            auto fileData = client.fetchFile (figmaFileKey, &errorMessage);

            if (onComplete != nullptr)
                onComplete (fileData, errorMessage);

            return jobHasFinished;
        }

    private:
        const String personalAccessToken, figmaFileKey;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FigmaLoadingJob)
    };

    class FigmaDisplayComponent final : public Component
    {
    public:
        FigmaDisplayComponent() = default;

        void setFigmaData (const var& fileData, FigmaClient& client, const String& fileKey)
        {
            if (fileData.isVoid())
            {
                setVisible (false);
                rootNode = nullptr;
                figmaClient = nullptr;
                return;
            }

            rootNode = client.parseDocumentTree (fileData);
            figmaClient = &client;
            figmaFileKey = fileKey;

            // Calculate bounds and set component size
            if (rootNode != nullptr && ! rootNode->children.isEmpty())
            {
                auto bounds = rootNode->children.getFirst()->bounds;
                for (const auto& child : rootNode->children)
                    bounds = bounds.getUnion (child.bounds);

                // Scale to fit reasonably in the demo
                const auto maxWidth     = 800.0f;
                const auto maxHeight    = 600.0f;
                const auto scaleX       = maxWidth / bounds.getWidth();
                const auto scaleY       = maxHeight / bounds.getHeight();
                const auto scale        = jmin (scaleX, scaleY, 1.0f);

                setSize ((int) (bounds.getWidth() * scale), (int) (bounds.getHeight() * scale));
                this->scale = scale;
            }

            setVisible (true);
            repaint();
        }

        void paint (Graphics& g) override
        {
            g.fillAll (Colours::darkgrey);

            if (rootNode == nullptr || rootNode->id.isEmpty())
                return;

            g.saveState();
            g.addTransform (AffineTransform::scale (scale));
            paintNode (g, *rootNode);
            g.restoreState();
        }

    private:
        std::unique_ptr<FigmaNode> rootNode;
        FigmaClient* figmaClient = nullptr;
        String figmaFileKey;
        float scale = 1.0f;

        void paintNode (Graphics& g, const FigmaNode& node)
        {
            const auto& bounds = node.bounds;

            // Paint fills
            for (const auto& fill : node.fills)
            {
                if (fill.type == FigmaPaint::Type::solid)
                {
                    g.setColour (fill.colour.withAlpha (fill.opacity));

                    if (node.type == "RECTANGLE")
                        g.fillRect (bounds);
                    else if (node.type == "ELLIPSE")
                        g.fillEllipse (bounds);
                }
                else if (fill.type == FigmaPaint::Type::linearGradient || fill.type == FigmaPaint::Type::radialGradient)
                {
                    g.setGradientFill (fill.gradient);

                    if (node.type == "RECTANGLE")
                        g.fillRect (bounds);
                    else if (node.type == "ELLIPSE")
                        g.fillEllipse (bounds);
                }
            }

            // Paint strokes
            if (! node.strokes.isEmpty() && node.strokeWeight > 0.0f)
            {
                Path path;
                if (node.type == "RECTANGLE")
                    path.addRectangle (bounds);
                else if (node.type == "ELLIPSE")
                    path.addEllipse (bounds);

                for (const auto& stroke : node.strokes)
                {
                    g.setColour (stroke.colour.withAlpha (stroke.opacity));
                    PathStrokeType strokeType (node.strokeWeight);
                    g.strokePath (path, strokeType);
                }
            }

            // Paint text
            if (node.type == "TEXT" && node.textContent.isNotEmpty())
            {
                Font font (node.textStyle.fontFamily, node.textStyle.fontSize, Font::plain);
                g.setFont (font);
                g.setColour (node.textStyle.fillColour);
                g.drawText (node.textContent, bounds, Justification::centred);
            }

            // Recursively paint children
            for (const auto& child : node.children)
                paintNode (g, *child);
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FigmaDisplayComponent)
    };

    //==============================================================================
    std::shared_ptr<ThreadPool> threadPool;

    TextEditor tokenInput;
    TextEditor fileKeyInput;
    TextButton loadButton;
    TextButton clearButton;
    Label statusLabel;
    FigmaDisplayComponent figmaComponent;

    std::unique_ptr<FigmaLoadingJob> loadingJob;
    std::unique_ptr<FigmaClient> figmaClient;

    //==============================================================================
    void loadFigmaFile()
    {
        const auto token = tokenInput.getText().trim();
        const auto fileKey = fileKeyInput.getText().trim();

        if (token.isEmpty() || fileKey.isEmpty())
        {
            statusLabel.setText (TRANS ("Please enter both token and file key"), dontSendNotification);
            statusLabel.setColour (Label::textColourId, Colours::orange);
            return;
        }

        if (loadingJob != nullptr)
        {
            if (! threadPool->removeJob (loadingJob.get(), true, 2000))
                return;
        }

        statusLabel.setText (TRANS ("Loading Figma file..."), dontSendNotification);
        statusLabel.setColour (Label::textColourId, Colours::lightblue);

        loadingJob = std::make_unique<FigmaLoadingJob> (token, fileKey);

        SafePointer sp (this);
        loadingJob->onComplete = [sp] (var fileData, String errorMessage)
        {
            MessageManager::callAsync ([sp, fileData, errorMessage]()
            {
                if (sp == nullptr)
                    return;

                if (errorMessage.isNotEmpty())
                {
                    sp->statusLabel.setText (TRANS ("Error: ") + errorMessage, dontSendNotification);
                    sp->statusLabel.setColour (Label::textColourId, Colours::red);
                    sp->figmaComponent.setVisible (false);
                }
                else
                {
                    sp->statusLabel.setText (TRANS ("File loaded successfully!"), dontSendNotification);
                    sp->statusLabel.setColour (Label::textColourId, Colours::lightgreen);

                    sp->figmaClient = std::make_unique<FigmaClient> (sp->tokenInput.getText().trim());
                    sp->figmaComponent.setFigmaData (fileData, *sp->figmaClient, sp->fileKeyInput.getText().trim());
                }

                sp->loadingJob = nullptr;
                sp->repaint();
            });
        };

        threadPool->addJob (loadingJob.get(), false);
    }

    void clearContent()
    {
        tokenInput.clear();
        fileKeyInput.clear();
        figmaComponent.setVisible (false);
        statusLabel.setText (TRANS ("Ready to load Figma file"), dontSendNotification);
        statusLabel.setColour (Label::textColourId, Colours::white);
        figmaClient = nullptr;
        repaint();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FigmaClientDemo)
};
