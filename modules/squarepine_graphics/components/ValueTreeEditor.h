//==============================================================================
/** */
class PropertyParser
{
public:
    /** */
    PropertyParser() = default;
    /** */
    virtual ~PropertyParser() = default;

    //==============================================================================
    /** @returns */
    virtual bool canUnderstand (const ValueTree& sourceTree, const Identifier& nameId, const var& prop) const = 0;
    /** @returns */
    virtual String toString (const Identifier& nameId, const var& prop) const = 0;
    /** @returns */
    virtual std::unique_ptr<PropertyComponent> createPropertyComponent (const Value& valueToControl,
                                                                        const Identifier& nameId) const = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyParser)
};

/** juce::File */
class FilePropertyParser final : public PropertyParser
{
public:
    /** */
    FilePropertyParser() = default;

    //==============================================================================
    /** @internal */
    bool canUnderstand (const ValueTree&, const Identifier&, const var& prop) const override
    {
        if (prop.isString())
            return File::isAbsolutePath (prop.toString());

        return false;
    }

    /** @internal */
    String toString (const Identifier&, const var& prop) const override
    {
        return VariantConverter<File>::fromVar (prop).getFullPathName();
    }

    /** @internal */
    std::unique_ptr<PropertyComponent> createPropertyComponent (const Value& valueToControl,
                                                                const Identifier& nameId) const
    {
        return std::make_unique<FilePropertyComponent> (valueToControl, nameId.toString());
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilePropertyParser)
};

/** juce::Rectangle<int> */
class RectanglePropertyParser final : public PropertyParser
{
public:
    /** */
    RectanglePropertyParser() = default;

    //==============================================================================
    /** @internal */
    bool canUnderstand (const ValueTree&, const Identifier& nameId, const var& prop) const override
    {
        if (! prop.isArray())
            return false;

        const auto& name = nameId.toString();

        // To not confused with the "rect" check below:
        if (name.containsIgnoreCase ("direction"))
            return false;

        return name.containsIgnoreCase ("bound")
            || name.containsIgnoreCase ("dim")
            || name.containsIgnoreCase ("rect");
    }

    /** @internal */
    String toString (const Identifier&, const var& prop) const override
    {
        return VariantConverter<juce::Rectangle<int>>::fromVar (prop).toString();
    }

    /** @internal */
    std::unique_ptr<PropertyComponent> createPropertyComponent (const Value& valueToControl,
                                                                const Identifier& nameId) const
    {
        return std::make_unique<RectanglePropertyComponent> (valueToControl, nameId.toString());
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RectanglePropertyParser)
};

/** Colour */
class ColourPropertyParser final : public PropertyParser
{
public:
    /** */
    ColourPropertyParser() = default;

    //==============================================================================
    /** @internal */
    bool canUnderstand (const ValueTree&, const Identifier& nameId, const var& prop) const override
    {
        if (! prop.isString())
            return false;

        const auto& name = nameId.toString();
        return name.containsIgnoreCase ("colour")
            || name.containsIgnoreCase ("color");
    }

    /** @internal */
    String toString (const Identifier&, const var& prop) const override
    {
        return VariantConverter<Colour>::fromVar (prop).toString();
    }

    /** @internal */
    std::unique_ptr<PropertyComponent> createPropertyComponent (const Value& valueToControl,
                                                                const Identifier& nameId) const
    {
        return std::make_unique<ColourPropertyComponent> (valueToControl, nameId.toString(), true);
    }

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourPropertyParser)
};

//==============================================================================
/** A component for viewing and editing a ValueTree's set of properties.

    Add PropertyParser derivatives by calling addPropertyParser() to
    allow handling and displaying custom PropertyComponent derivatives.
*/
class ValueTreeEditor final : public Component
{
public:
    /** */
    ValueTreeEditor() :
        layoutResizer (&layout, 1, false)
    {
        layout.setItemLayout (0, -0.1, -0.9, -0.6);
        layout.setItemLayout (1, 8.0, 8.0, 8.0);
        layout.setItemLayout (2, -0.1, -0.9, -0.4);

        addAndMakeVisible (treeView);
        addAndMakeVisible (propertyEditor);
        addAndMakeVisible (layoutResizer);

        addDefaultParsers();
    }

    /** */
    ValueTreeEditor (const ValueTree& startingSource) :
        ValueTreeEditor()
    {
        setSource (startingSource);
    }

    /** */
    ~ValueTreeEditor() override
    {
        treeView.deleteRootItem();
    }

    //==============================================================================
    /** */
    void setSource (const ValueTree& newTree)
    {
        if (! newTree.isValid())
        {
            treeView.deleteRootItem();
        }
        else if (tree != newTree)
        {
            treeView.deleteRootItem();
            tree = newTree;
            treeView.setRootItem (new Item (*this, propertyEditor, tree));
        }
    }

    /** */
    void addPropertyParser (std::unique_ptr<PropertyParser> pp)
    {
        jassert (pp != nullptr);
        if (pp != nullptr)
            parsers.add (pp.release());
    }

    /** */
    void addDefaultParsers()
    {
        addPropertyParser (std::make_unique<FilePropertyParser>());
        addPropertyParser (std::make_unique<RectanglePropertyParser>());
        addPropertyParser (std::make_unique<ColourPropertyParser>());
    }

    /** */
    void removePropertyParser (PropertyParser* pp)
    {
        jassert (pp != nullptr);
        jassert (parsers.contains (pp));

        if (pp != nullptr)
            parsers.removeObject (pp);
    }

    /** */
    int getNumParsers() const noexcept { return parsers.size(); }

    /** */
    PropertyParser* getParser (int index) const noexcept { return parsers[index]; }

    /** */
    void clearParsers() { parsers.clear(); }

    //==============================================================================
    /** @internal */
    void resized() override
    {
        Component* comps[] = { &treeView, &layoutResizer, &propertyEditor };

        layout.layOutComponents (comps, 3, 0, 0, getWidth(), getHeight(), true, true);
    }

private:
    //==============================================================================
    /** Display properties for a tree. */
    class PropertyEditor final : public PropertyPanel
    {
    public:
        PropertyEditor() = default;

        static String toString (const OwnedArray<PropertyParser>& parsers,
                                const ValueTree& sourceTree,
                                const Identifier& name,
                                const var& prop)
        {
            for (auto* pp : parsers)
                if (pp->canUnderstand (sourceTree, name, prop))
                    return pp->toString (name, prop);

            if (prop.isBool())          return booleanToString (static_cast<bool> (prop)).toLowerCase();
            if (prop.isString())        return prop.toString();
            if (prop.isVoid())          return "(void)";
            if (prop.isMethod())        return "(method)";
            if (prop.isObject())        return "(object)";
            if (prop.isBinaryData())    return "(binary data)";
            if (prop.isArray())         return "(array)";

            return prop.toString();
        }

        void setSource (ValueTree& newSource, const OwnedArray<PropertyParser>& parsers)
        {
            clear();

            tree = newSource;

            constexpr int maxChars = 200;
            Array<PropertyComponent*> pc;

            for (int i = 0; i < tree.getNumProperties(); ++i)
            {
                const auto name     = tree.getPropertyName (i);
                const auto value    = tree.getPropertyAsValue (name, nullptr);
                const auto prop     = value.getValue();

                auto* tpc = [&]() -> PropertyComponent*
                {
                    for (auto* pp : parsers)
                        if (pp->canUnderstand (tree, name, prop))
                            return pp->createPropertyComponent (value, name).release();

                    return {};
                }();

                if (tpc == nullptr)
                {
                    const auto& nameStr = name.toString();

                    if (prop.isObject())
                    {
                        tpc = new TextPropertyComponent (noEditValue, nameStr, maxChars, false, false);
                        tpc->setEnabled (false);
                    }
                    else if (prop.isBool())
                    {
                        tpc = new BooleanPropertyComponent (value, nameStr, {});
                    }
                    else
                    {
                        tpc = new TextPropertyComponent (value, nameStr, maxChars, false);
                    }
                }

                pc.add (tpc);
            }

            addProperties (pc, 1);
        }

    private:
        const Value noEditValue { var ("(Not Editable)") };
        ValueTree tree;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyEditor)
    };

    //==============================================================================
    class Item final : public TreeViewItem,
                       public ValueTree::Listener
    {
    public:
        Item (ValueTreeEditor& parentVTE,
              PropertyEditor& pe,
              const ValueTree& sourceTree) :
            valueTreeEditor (parentVTE),
            propertiesEditor (pe),
            tree (sourceTree)
        {
            tree.addListener (this);
        }

        ~Item() override
        {
            tree.removeListener (this);
            clearSubItems();
        }

        void itemOpennessChanged (bool isNowOpen) override
        {
            clearSubItems();

            if (isNowOpen)
                for (const auto& child : tree)
                    addSubItem (new Item (valueTreeEditor, propertiesEditor, child));
        }

        void paintItem (Graphics& g, int w, int h) override
        {
            if (isSelected())
            {
                g.fillAll (propertiesEditor.findColour (TextEditor::ColourIds::highlightColourId));
                g.setColour (propertiesEditor.findColour (TextEditor::ColourIds::highlightedTextColourId));
            }
            else
            {
                g.setColour (propertiesEditor.findColour (TextEditor::ColourIds::textColourId));
            }

            auto text = tree.getType().toString();
            if (const auto numProps = tree.getNumProperties(); numProps > 0)
                text << ", numProps: " << numProps;

            if (const auto numChildren = tree.getNumChildren(); numChildren > 0)
                text << ", numChildren: " << numChildren;

            g.setFont (static_cast<float> (h) * 0.9f);
            g.drawText (text, 0, 0, w, h, Justification::centredLeft, false);
        }

        void itemSelectionChanged (bool isNowSelected) override
        {
            if (isNowSelected)
            {
                tree.removeListener (this);
                propertiesEditor.setSource (tree, valueTreeEditor.parsers);
                tree.addListener (this);
            }
        }

        void valueTreePropertyChanged (ValueTree& vt, const Identifier&) override
        {
            if (tree != vt)
                return;

            tree.removeListener (this);

//            if (isSelected())
//                propertiesEditor.setSource (tree);

            repaintItem();
            tree.addListener (this);
        }

        bool mightContainSubItems() override                                { return tree.getNumChildren() > 0; }
        void valueTreeChildAdded (ValueTree&, ValueTree&) override          { treeHasChanged(); }
        void valueTreeChildRemoved (ValueTree&, ValueTree&, int) override   { treeHasChanged(); }
        void valueTreeChildOrderChanged (ValueTree&, int, int) override     { treeHasChanged(); }
        void valueTreeParentChanged (ValueTree&) override                   { treeHasChanged(); }
        void valueTreeRedirected (ValueTree&) override                      { treeHasChanged(); }

    private:
        ValueTreeEditor& valueTreeEditor;
        PropertyEditor& propertiesEditor;
        ValueTree tree;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Item)
    };

    //==============================================================================
    OwnedArray<PropertyParser> parsers;
    ValueTree tree;
    TreeView treeView;
    PropertyEditor propertyEditor;
    StretchableLayoutManager layout;
    StretchableLayoutResizerBar layoutResizer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeEditor)
};
