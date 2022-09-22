/** */
class ImageDemo final : public DemoBase,
                        public FileDragAndDropTarget,
                        public TextDragAndDropTarget
{
public:
    /** */
    ImageDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Image Format Demo"))
    {
        imageFormatManager.registerBasicFormats();

        addAndMakeVisible (imageComponent);
    }

    void paint (Graphics& g) override
    {
        if (imageComponent.getImage().isNull())
            sp::drawFittedText (g, TRANS ("Drop an image or Base64 image text here!"), getLocalBounds(), Justification::centred);
    }

    void resized() override
    {
        imageComponent.setBounds (getLocalBounds ());
    }

    bool isInterestedInTextDrag (const String& text) override
    {
        // constexpr char base64Lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        // return text.contains (base64Lookup);

        // TODO find algorithm to check if string is containing only Base64 characters.
        ignoreUnused (text);
        return false;
    }

    bool isInterestedInFileDrag (const StringArray& files) override
    {
        for (const auto& path : files)
            if (imageFormatManager.findFormatForFile (File (path)) != nullptr)
                return true;

        return false;
    }

    void filesDropped (const StringArray& files, int, int) override
    {
        for (const auto& path : files)
        {
            const File file (path);
            if (imageFormatManager.findFormatForFile (file) != nullptr)
            {
                imageComponent.setImage (imageFormatManager.loadFrom (file));
                repaint();
                return;
            }
        }
    }

    void textDropped (const String& text, int, int) override
    {
        // TODO find algorithm to check if string is containing only Base64 characters.
        ignoreUnused (text);
    }

private:
    sp::ImageFormatManager imageFormatManager;
    sp::HighQualityImageComponent imageComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageDemo)
};
