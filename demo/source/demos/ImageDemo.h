/** */
class ImageDemo final : public DemoBase,
                        public FileDragAndDropTarget,
                        public TextDragAndDropTarget
{
public:
    /** */
    ImageDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Image Formats"))
    {
        imageFormatManager.registerBasicFormats();

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
                                                                 imageFormatManager.getWildcardForAllFormats());

                    fileChooser->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                              [sp] (const FileChooser& chooser)
                    {
                        auto c = std::move (sp->fileChooser);
                        ignoreUnused (c);

                        const auto newFileURL = chooser.getURLResult();
                        if (sp == nullptr || newFileURL.isEmpty())
                            return; // User cancelled.

                        jassert (newFileURL.isLocalFile());

                        sp->setImage (newFileURL);
                    });
                }
            );
        };

        addAndMakeVisible (imageComponent);
        addAndMakeVisible (open);
    }

    //==============================================================================
    /** Changes the currently displayed with the given one.
        If the given image is null, the underlying image component will be cleared.
    */
    void setImage (const Image& image)
    {
        imageComponent.setImage (image);
        repaint();
    }

    /** Changes the currently displayed with one from the given file.
        If the given image fails to load, the underlying image component will be cleared.
    */
    void setImage (const File& file)
    {
        setImage (imageFormatManager.loadFrom (file));
    }

    /** Changes the currently displayed with one from the given URL.
        If the given image fails to load, the underlying image component will be cleared.
    */
    void setImage (const URL& url)
    {
        setImage (imageFormatManager.loadFrom (url));
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
        auto b = getLocalBounds().reduced (marginPx);

        auto topBar = b.removeFromTop (barSizePx);
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
                if (imageFormatManager.findFormatForFile (File (path)) != nullptr)
                    return true;

        return false;
    }

    /** @internal */
    void filesDropped (const StringArray& files, int, int) override
    {
        for (const auto& path : files)
        {
            const File file (path);
            if (file.existsAsFile() && imageFormatManager.findFormatForFile (file) != nullptr)
            {
                setImage (file);
                return;
            }
        }
    }

    /** @internal */
    void textDropped (const String& text, int, int) override
    {
        if (isPossiblyBase64 (text))
            setImage (imageFormatManager.fromBase64 (text));
    }

    void updateWithNewTranslations() override
    {
        open.setButtonText (TRANS ("Open Image File"));
        repaint();
    }

private:
    //==============================================================================
    ImageFormatManager imageFormatManager;
    HighQualityImageComponent imageComponent;
    TextButton open;
    std::unique_ptr<FileChooser> fileChooser;

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

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageDemo)
};
