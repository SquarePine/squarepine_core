//==============================================================================
/** */
class ImageLoadingJob final : public ThreadPoolJob
{
public:
    /** Constructor. */
    ImageLoadingJob (const URL& urlToLoad,
                     std::shared_ptr<ImageFormatManager> imageFormatManagerToUse,
                     const String& jobName = "ImageLoadingJob") :
        ThreadPoolJob (jobName),
        url (urlToLoad),
        imageFormatManager (imageFormatManagerToUse)
    {
        jassert (url.isWellFormed());
        jassert (imageFormatManager != nullptr);
    }

    //==============================================================================
    std::function<void(Image, bool)> onComplete;

    const URL& getURL() const noexcept { return url; }
    const Image& getImage() const noexcept { return image; }

    //==============================================================================
    JobStatus runJob() override
    {
        image = imageFormatManager->loadFrom (url);

        if (onComplete != nullptr)
            onComplete (image, image.isValid());

        return jobHasFinished;
    }

private:
    //==============================================================================
    const URL url;
    Image image;
    std::shared_ptr<ImageFormatManager> imageFormatManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageLoadingJob)
};

//==============================================================================
/** */
class ImageDemo final : public DemoBase,
                        public FileDragAndDropTarget,
                        public TextDragAndDropTarget
{
public:
    /** */
    ImageDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Image Formats")),
        imageFormatManager (sharedObjs.imageFormatManager)
    {
        SafePointer sp (this);

        open.onClick = [this, sp]()
        {
            if (sp == nullptr || fileChooser != nullptr)
                return;

            RuntimePermissions::request (RuntimePermissions::readMediaImages,
                [this, sp] (bool wasGranted)
                {
                    if (! wasGranted)
                        return;

                    fileChooser = std::make_unique<FileChooser> (TRANS ("Find an image to load."),
                                                                 File::getSpecialLocation (File::userPicturesDirectory),
                                                                 imageFormatManager->getWildcardForAllFormats());

                    fileChooser->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                              [sp] (const FileChooser& chooser)
                    {
                        auto c = std::move (sp->fileChooser);
                        ignoreUnused (c);

                        const auto newFileURL = chooser.getURLResult();
                        if (sp == nullptr || newFileURL.isEmpty())
                            return; // User cancelled.

                        jassert (newFileURL.isLocalFile());

                        sp->setImage (newFileURL, true);
                    });
                }
            );
        };

        addAndMakeVisible (imageComponent);
        addAndMakeVisible (open);
    }

    ~ImageDemo() override
    {
        if (imageLoadingJob != nullptr
            && ! sharedObjects.threadPool->removeJob (imageLoadingJob.get(), true, 5000))
        {
            jassertfalse;
        }
    }

    //==============================================================================
    void setImage (const Image& image)
    {
        imageComponent.setImage (image);
        repaint();
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        if (imageComponent.getImage().isNull())
        {
            g.setColour (Colours::white);
            sp::drawFittedText (g, TRANS ("Drop an image or Base64 image text here!"), getLocalBounds(), Justification::centred);
        }
    }

    /** @internal */
    void resized() override
    {
        auto b = getLocalBounds().reduced (dims::marginPx);

        auto topBar = b.removeFromTop (dims::barSizePx);
        open.setBounds (topBar.removeFromLeft (192));

        imageComponent.setBounds (b);
    }

    /** @internal */
    bool isInterestedInTextDrag (const String& text) override { return isPossiblyBase64 (text); }

    /** @internal */
    bool isInterestedInFileDrag (const StringArray& files) override
    {
        for (const auto& path : files)
            if (const File file (path); file.existsAsFile())
                if (imageFormatManager->findFormatForFile (File (path)) != nullptr)
                    return true;

        return false;
    }

    /** @internal */
    void filesDropped (const StringArray& files, int, int) override
    {
        for (const auto& path : files)
        {
            const File file (path);
            if (file.existsAsFile() && imageFormatManager->findFormatForFile (file) != nullptr)
            {
                setImage (URL (file), true);
                return;
            }
        }
    }

    /** @internal */
    void textDropped (const String& text, int, int) override
    {
        if (isPossiblyBase64 (text))
            setImage (imageFormatManager->fromBase64 (text));
    }

    void updateWithNewTranslations() override
    {
        open.setButtonText (TRANS ("Open Image File"));
        repaint();
    }

private:
    //==============================================================================
    struct AnimatedSpinner final : public Component,
                                   private Timer
    {
        AnimatedSpinner()
        {
            setInterceptsMouseClicks (false, true);
            startTimerHz (60);
        }

        void timerCallback() override { repaint(); }

        void paint (Graphics& g) override
        {
            getLookAndFeel().drawSpinningWaitAnimation (g, Colours::darkgrey, 0, 0, getWidth(), getHeight());
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimatedSpinner)
    };

    std::shared_ptr<ImageFormatManager> imageFormatManager;
    HighQualityImageComponent imageComponent;
    TextButton open;
    std::unique_ptr<FileChooser> fileChooser;
    std::unique_ptr<ImageLoadingJob> imageLoadingJob;
    URL lastURL;

    std::unique_ptr<AnimatedSpinner> animatedSpinner;

    //==============================================================================
    static bool isPossiblyBase64 (const String& text)
    {
        const String b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        for (auto cp = text.getCharPointer();;)
        {
            const auto c = cp.getAndAdvance();
            if (c == 0 || ! b64.containsChar (c))
                return false;
        }

        return true;
    }

    template <typename Callback>
    static void callOnMessageThread (Callback&& callback)
    {
        if (MessageManager::getInstance()->isThisTheMessageThread())
            callback();
        else
            MessageManager::callAsync (std::forward<Callback> (callback));
    }

    void startSpinnerAnimation()
    {
        animatedSpinner = std::make_unique<AnimatedSpinner>();
        animatedSpinner->setBounds (getLocalBounds());
        addAndMakeVisible (animatedSpinner.get());
    }

    void stopSpinnerAnimation()
    {
        animatedSpinner = nullptr;
    }

    bool waitForImageLoadingJobToComplete()
    {
        if (imageLoadingJob != nullptr)
        {
            imageLoadingJob->signalJobShouldExit();

            if (sharedObjects.threadPool->removeJob (imageLoadingJob.get(), true, 5000))
            {
                imageLoadingJob = nullptr;
            }
        }

        return imageLoadingJob == nullptr;
    }

    void setImage (const URL& url, bool async)
    {
        if (lastURL == url)
            return;

        lastURL = url;

        if (async)
            startLoadingAsync();
        else
            setImage (imageFormatManager->loadFrom (url));
    }

    void startLoadingAsync()
    {
        if (! waitForImageLoadingJobToComplete())
            return;

        setImage ({}); // Clear the image.
        startSpinnerAnimation();

        imageLoadingJob = std::make_unique<ImageLoadingJob> (lastURL, sharedObjects.imageFormatManager);

        SafePointer sp (this);
        imageLoadingJob->onComplete = [sp] (Image image, bool succeeded)
        {
            callOnMessageThread ([sp, image, succeeded]()
            {
                if (sp != nullptr)
                {
                    if (succeeded)
                        sp->setImage (image);

                    sp->stopSpinnerAnimation();
                    sp->waitForImageLoadingJobToComplete();
                }
            }); 
        };

        sharedObjects.threadPool->addJob (imageLoadingJob.get(), false);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageDemo)
};
