#if SP_DEMO_USE_OPENGL

class OpenGLDetailsDemo final : public DemoBase,
                                public ListBoxModel
{
public:
    OpenGLDetailsDemo (SharedObjects& sharedObjs, HighPerformanceRendererConfigurator& rc) :
        DemoBase (sharedObjs, NEEDS_TRANS ("OpenGL Details")),
        rendererConfigurator (rc)
    {
        SafePointer ptr (this);

        searchBox.onTextChange = [ptr]()
        {
            if (ptr != nullptr)
                ptr->refresh();
        };

        searchBox.setJustification (Justification::centredLeft);
        searchBox.setTextToShowWhenEmpty (TRANS ("(Type here to search)"), Colours::grey);

        addAndMakeVisible (searchBox);

        listbox.setModel (this);
        addAndMakeVisible (listbox);

        MessageManager::callAsync ([ptr]()
        {
            if (ptr == nullptr)
                return;

            if (auto* contextPtr = ptr->rendererConfigurator.context.get())
                contextPtr->executeOnGLThread ([ptr] (OpenGLContext&)
                {
                    if (ptr == nullptr)
                        return;

                    // Can only be retrieved on the GL thread.
                    const auto details = ptr->getOpenGLDetails();

                    // Can only be set on the main message thread.
                    MessageManager::callAsync ([ptr, details]()
                    {
                        if (ptr != nullptr)
                        {
                            ptr->systemDetails = details;
                            ptr->refresh();
                        }
                    });
                }, false);
        });
    }

    void resized() override
    {
        auto b = getLocalBounds().reduced (4);

        searchBox.setBounds (b.removeFromTop (32));
        b.removeFromTop (4);
        listbox.setBounds (b);
    }

    int getNumRows() override { return displayDetails.size(); }

    static String getNameForRow (const StringPairArray& details, int row)
    {
        const auto key = details.getAllKeys()[row];
        const auto& value = details[key];
        return key + ": " + value;
    }

    String getNameForRow (int row) override
    {
        jassert (isPositiveAndBelow (row, getNumRows()));
        return getNameForRow (displayDetails, row);
    }

    void paintListBoxItem (int row, Graphics& g, int w, int h, bool) override
    {
        if (! isPositiveAndBelow (row, getNumRows()))
            return;

        const auto text = getNameForRow (row);
        g.setColour (findColour (ListBox::textColourId, true));
        g.drawFittedText (text, { w, h }, Justification::centredLeft, 1, 1.0f);
    }

private:
    HighPerformanceRendererConfigurator& rendererConfigurator;
    StringPairArray systemDetails, displayDetails;
    TextEditor searchBox;
    ListBox listbox;

    void refresh()
    {
        const auto searchText = searchBox.getText();

        displayDetails.clear();

        for (int i = 0; i < systemDetails.size(); ++i)
        {
            const auto key = systemDetails.getAllKeys()[i];
            const auto& value = systemDetails[key];

            if (key.containsIgnoreCase (searchText)
                || value.containsIgnoreCase (searchText))
            {
                displayDetails.set (key, value);
            }
        }

        listbox.updateContent();
    }

    StringPairArray getOpenGLDetails() const
    {
        using namespace juce::gl;

        StringPairArray result;

        auto addStringResult = [&] (const String& name, const String& details)
        {
            jassert (! result.containsKey (name));

            if (details.isNotEmpty())
                result.set (name, details);
        };

        auto addResult = [&] (const String& name, GLenum value)
        {
            addStringResult (name, sp::getGLString (value).trim());
        };

        #undef SP_ADD_LOG_ITEM
        #define SP_ADD_LOG_ITEM(x) \
            addResult (#x, x);

        SP_ADD_LOG_ITEM (GL_VENDOR)
        SP_ADD_LOG_ITEM (GL_RENDERER)
        SP_ADD_LOG_ITEM (GL_VERSION)
        SP_ADD_LOG_ITEM (GL_SHADING_LANGUAGE_VERSION)
       #if ! JUCE_ANDROID
        SP_ADD_LOG_ITEM (GL_PROGRAM_ERROR_STRING_NV)
       #endif

        GLint major = 0, minor = 0, numExtensions = 0;
        glGetIntegerv (GL_MAJOR_VERSION, &major);
        glGetIntegerv (GL_MINOR_VERSION, &minor);
        glGetIntegerv (GL_NUM_EXTENSIONS, &numExtensions);

        addStringResult ("GL_MAJOR_VERSION", String (major));
        addStringResult ("GL_MINOR_VERSION", String (minor));
        addStringResult ("GL_NUM_EXTENSIONS", String (numExtensions));

        const auto extensions = sp::getOpenGLExtensions (getPeer());
        int index = 1;
        for (const auto& ext : extensions)
            addStringResult ("Extension " + String (index++), ext);

        result.minimiseStorageOverheads();
        return result;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLDetailsDemo)
};

#endif // SP_DEMO_USE_OPENGL
