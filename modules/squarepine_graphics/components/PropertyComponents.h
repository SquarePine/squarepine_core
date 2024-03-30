//==============================================================================
/** */
class PropertyComponentBase : public PropertyComponent,
                              private Value::Listener
{
public:
    PropertyComponentBase (const Value& valueToControl,
                           const String& propertyName,
                           int preferredHeight = 32) :
        PropertyComponent (propertyName, preferredHeight),
        value (valueToControl)
    {
        value.addListener (this);
    }

protected:
    //==============================================================================
    /** @internal */
    void valueChanged (Value&) override { refresh(); }

    //==============================================================================
    Value value;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyComponentBase)
};

//==============================================================================
/** */
class FilePropertyComponent final : public PropertyComponentBase
{
public:
    /** */
    FilePropertyComponent (const Value& valueToControl, const String& propertyName,
                           const String& title = TRANS ("Open"), const String& pattern = "*.*") :
        PropertyComponentBase (valueToControl, propertyName),
        chooser (title, File (valueToControl.toString()), pattern)
    {
        container.browse.onClick = [this]
        {
            constexpr int flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories;

            chooser.launchAsync (flags, [this] (const FileChooser&)
            {
                value.setValue (chooser.getResult().getFullPathName());
            });
        };

        container.clear.onClick = [this] { value.setValue (String()); };
        addAndMakeVisible (container);
    }

    //==============================================================================
    /** @internal */
    void refresh() override { container.filename.setText (value.toString()); }

private:
    //==============================================================================
    class Container final : public Component
    {
    public:
        Container()
        {
            filename.setReadOnly (true);

            addAndMakeVisible (filename);
            addAndMakeVisible (browse);
            addAndMakeVisible (clear);
        }

        //==============================================================================
        void resized() override
        {
            auto rc = getLocalBounds();
            clear.setBounds (rc.removeFromRight (rc.getHeight()));
            browse.setBounds (rc.removeFromRight (rc.getHeight()));
            rc.removeFromRight (3);
            filename.setBounds (rc);
        }

        //==============================================================================
        TextEditor filename;
        TextButton browse { "..." }, clear { "X" };

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
    };

    //==============================================================================
    FileChooser chooser;
    Container container;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilePropertyComponent)
};

//==============================================================================
/** */
class ColourPropertyComponent final : public PropertyComponentBase
{
public:
    /** */
    ColourPropertyComponent (const Value& valueToControl,
                             const String& propertyName,
                             bool showAlpha = false,
                             int preferredHeight = 64) :
        PropertyComponentBase (valueToControl, propertyName, preferredHeight),
        container (value, showAlpha)
    {
        addAndMakeVisible (container);
    }

    //==============================================================================
    /** @internal */
    void refresh() override { repaint(); }

    /** @internal */
    void paint (Graphics& g) override
    {
        PropertyComponent::paint (g);

        g.setColour (findColour (PropertyComponent::backgroundColourId));
        g.fillRect (container.getBounds());

        g.setColour (findColour (BooleanPropertyComponent::outlineColourId));
        g.drawRect (container.getBounds());
    }

private:
    //==============================================================================
    class Container final : public Component,
                            private ChangeListener
    {
    public:
        Container (Value& v, bool a) :
            value (v),
            alpha (a)
        {
        }

        //==============================================================================
        /** @returns */
        Colour getCurrentColour() const { return VariantConverter<Colour>::fromVar (value.getValue()); }

        //==============================================================================
        /** @internal */
        void paint (Graphics& g) override
        {
            constexpr auto margin = 4;
            auto b = getLocalBounds().reduced (margin);
            const auto c = getCurrentColour();
            g.setColour (c);
            g.fillRect (b);

            b = b.reduced (margin);
            g.setFont ((float) b.reduced (margin / 2).getHeight());
            g.setColour (c.contrasting());
            g.drawText ("0x" + c.toDisplayString (alpha), b, Justification::centredLeft);
        }

        /** @internal */
        void mouseDown (const MouseEvent&) override
        {
            int csflags = ColourSelector::showColourAtTop
                        | ColourSelector::editableColour
                        | ColourSelector::showSliders
                        | ColourSelector::showColourspace;

            if (alpha)
                csflags |= ColourSelector::showAlphaChannel;

            auto colourSelector = std::make_unique<ColourSelector> (csflags);
            colourSelector->setName ("Colour");
            colourSelector->setCurrentColour (getCurrentColour());
            colourSelector->addChangeListener (this);
            colourSelector->setColour (ColourSelector::backgroundColourId, findColour (PropertyComponent::backgroundColourId));
            colourSelector->setSize (400, 400);

            CallOutBox::launchAsynchronously (std::move (colourSelector), getScreenBounds(), nullptr);
        }

        /** @internal */
        void changeListenerCallback (ChangeBroadcaster* source) override
        {
            if (auto* cs = dynamic_cast<ColourSelector*> (source))
            {
                value.setValue (VariantConverter<Colour>::toVar (cs->getCurrentColour()));
                repaint();
            }
        }

    private:
        //==============================================================================
        Value& value;
        const bool alpha;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
    };

    //==============================================================================
    Container container;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPropertyComponent)
};

//==============================================================================
/** */
class RectanglePropertyComponent final : public PropertyComponentBase
{
public:
    /** */
    RectanglePropertyComponent (const Value& valueToControl, const String& propertyName) :
        PropertyComponentBase (valueToControl, propertyName)
    {

    }

    //==============================================================================
    /** @internal */
    void refresh() override
    {
        // editor.setText (VariantConverter<juce::Rectangle<int>>::fromVar (value.getValue()).toString());
    }

private:
    //==============================================================================
    struct Combo final
    {
        Combo (RectanglePropertyComponent& rpc, int indexToUse) :
            parent (rpc),
            index (indexToUse)
        {
            editor.onReturnKey = [this]() { parent.applyTextChange (editor, index); };
            editor.onFocusLost = [this]() { parent.applyTextChange (editor, index); };

            parent.addAndMakeVisible (label);
            parent.addAndMakeVisible (editor);
        }

        RectanglePropertyComponent& parent;
        const int index;
        Label label;
        TextEditor editor;
    };

    friend Combo;
    OwnedArray<Combo> combos;

    //==============================================================================
    void applyTextChange (TextEditor& editor, int index)
    {
        const auto strings = VariantConverter<juce::Rectangle<int>>::fromVar (value.getValue()).toString();
        auto tokens = StringArray::fromTokens (strings, " ", "");
        tokens.set (index, editor.getText());

        const auto rect = juce::Rectangle<int>::fromString (tokens.joinIntoString (" "));
        value = VariantConverter<juce::Rectangle<int>>::toVar (rect);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RectanglePropertyComponent)
};
