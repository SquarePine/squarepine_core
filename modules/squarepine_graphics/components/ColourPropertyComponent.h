//==============================================================================
class PropertyComponentBase : public PropertyComponent,
                              private Value::Listener
{
public:
    PropertyComponentBase (const Value& valueToControl, const String& propertyName) :
        PropertyComponent (propertyName), value (valueToControl)
    {
        value.addListener (this);
    }

protected:
    //==============================================================================
    void valueChanged (Value&) override { refresh(); }

    //==============================================================================
    Value value;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyComponentBase)
};

//==============================================================================
class FilePropertyComponent final : public PropertyComponentBase
{
public:
    FilePropertyComponent (const Value& valueToControl, const String& propertyName, const String& title_ = "Open", const String pattern_ = "*.*") :
        PropertyComponentBase (valueToControl, propertyName),
        title (title_),
        pattern (pattern_)
    {
        addAndMakeVisible (container);

        container.browse.onClick = [this]
        {
            FileChooser box (title, File (value.toString()), pattern);

            if (box.browseForFileToOpen())
                value.setValue (box.getResult().getFullPathName());
        };

        container.clear.onClick = [this] { value.setValue (String()); };
    }

    //==============================================================================
    void refresh() override { container.filename.setText (value.toString()); }

private:
    //==============================================================================
    class Container final : public Component
    {
    public:
        Container()
        {
            addAndMakeVisible (filename);
            addAndMakeVisible (browse);
            addAndMakeVisible (clear);

            filename.setReadOnly (true);
        }

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
        TextButton browse { "..." };
        TextButton clear { "X" };

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
    };

    //==============================================================================
    Container container;
    String title, pattern;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilePropertyComponent)
};

//==============================================================================
class ColourPropertyComponent final : public PropertyComponentBase
{
public:
    ColourPropertyComponent (const Value& valueToControl, const String& propertyName, bool showAlpha = false) :
        PropertyComponentBase (valueToControl, propertyName),
        container (value, showAlpha)
    {
        addAndMakeVisible (container);
    }

    //==============================================================================
    void refresh() override { repaint(); }

    void paint (Graphics& g) override
    {
        PropertyComponent::paint (g);

        g.setColour (findColour (BooleanPropertyComponent::backgroundColourId));
        g.fillRect (container.getBounds());

        g.setColour (findColour (BooleanPropertyComponent::outlineColourId));
        g.drawRect (container.getBounds());
    }

private:
    //==============================================================================
    class Container final : public Component
    {
    public:
        Container (Value& v, bool a) :
            value (v),
            alpha (a)
        {
        }

        Colour getCurrentColour() const
        {
            return VariantConverter<Colour>::fromVar (value.getValue());
        }

        void paint (Graphics& g) override
        {
            const auto c = getCurrentColour();

            g.setColour (c);
            g.fillRect (getLocalBounds().reduced (4));

            g.setColour (c.contrasting());
            g.drawText (c.toDisplayString (alpha), getLocalBounds(), Justification::centred);
        }

        void mouseUp (const MouseEvent& e) override
        {
            ignoreUnused (e);

           #if JUCE_MODAL_LOOPS_PERMITTED
            if (e.mouseWasClicked())
            {
                ColourSelector colourSelector (ColourSelector::showColourAtTop | ColourSelector::showSliders | ColourSelector::showColourspace);

                colourSelector.setSize (300, 280);
                colourSelector.setCurrentColour (getCurrentColour(), dontSendNotification);

                CallOutBox callOut (colourSelector, getScreenBounds(), nullptr);
                callOut.runModalLoop();

                value = colourSelector.getCurrentColour().toString();
            }
           #endif
        }

    private:
        //==============================================================================
        Value& value;
        bool alpha;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Container)
    };

    //==============================================================================
    Container container;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPropertyComponent)
};

