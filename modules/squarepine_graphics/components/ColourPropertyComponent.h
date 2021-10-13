//==============================================================================
/** */
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
    ColourPropertyComponent (const Value& valueToControl, const String& propertyName, bool showAlpha = false) :
        PropertyComponentBase (valueToControl, propertyName),
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

        g.setColour (findColour (BooleanPropertyComponent::backgroundColourId));
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
            const auto c = getCurrentColour();

            g.setColour (c);
            g.fillRect (getLocalBounds().reduced (4));

            g.setColour (c.contrasting());
            g.drawText (c.toDisplayString (alpha), getLocalBounds(), Justification::centred);
        }

        /** @internal */
        void mouseUp (const MouseEvent& e) override
        {
            if (e.mouseWasClicked())
            {
               #if JUCE_MODAL_LOOPS_PERMITTED
                ColourSelector colourSelector (ColourSelector::showColourAtTop | ColourSelector::showSliders | ColourSelector::showColourspace);

                colourSelector.setSize (300, 280);
                colourSelector.setCurrentColour (getCurrentColour(), dontSendNotification);

                CallOutBox callOut (colourSelector, getScreenBounds(), nullptr);
                callOut.runModalLoop();

                value = colourSelector.getCurrentColour().toString();
               #else
                jassertfalse; // TODO
               #endif
            }
        }

        void mouseDown (const MouseEvent&) override
        {
#if 0
            auto colourSelector = std::make_unique<ColourSelector>();
            colourSelector->setName ("Colour");
            colourSelector->setCurrentColour (getColour());
            colourSelector->addChangeListener (this);
            colourSelector->setColour (ColourSelector::backgroundColourId, Colours::transparentBlack);
            colourSelector->setSize (300, 400);

            CallOutBox::launchAsynchronously (std::move (colourSelector), getScreenBounds(), nullptr);

#endif
        }

        void changeListenerCallback (ChangeBroadcaster* source) override
        {
            ignoreUnused (source);

#if 0
            if (auto* cs = dynamic_cast<ColourSelector*> (source))
                editor.applyNewValue (getAsString (cs->getCurrentColour(), true));

            repaint();
#endif
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
