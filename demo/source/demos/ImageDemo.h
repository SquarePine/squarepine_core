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

        addAndMakeVisible (imageComponent);
        addAndMakeVisible (open);
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
    bool isInterestedInTextDrag (const String& text) override { return isBase64 (text); }

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
                setImage (imageFormatManager.loadFrom (file));
                return;
            }
        }
    }

    /** @internal */
    void textDropped (const String& text, int, int) override
    {
        if (isBase64 (text))
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

    //==============================================================================
    void setImage (const Image& image)
    {
        imageComponent.setImage (image);
        repaint();
    }

    static bool isBase64 (const String& text)
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
