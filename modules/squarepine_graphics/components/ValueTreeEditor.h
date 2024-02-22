/** Displays a separate desktop window for viewing and editing a ValueTree's set of properties. */
class ValueTreeEditor final : public Component
{
public:
    /** */
    ValueTreeEditor() :
        layoutResizer (&layout, 1, false)
    {
        setOpaque (true);

        layout.setItemLayout (0, -0.1, -0.9, -0.6);
        layout.setItemLayout (1, 8.0, 8.0, 8.0);
        layout.setItemLayout (2, -0.1, -0.9, -0.4);

        addAndMakeVisible (treeView);
        addAndMakeVisible (propertyEditor);
        addAndMakeVisible (layoutResizer);
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
            treeView.setRootItem (new Item (propertyEditor, tree));
        }
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::white);
    }

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
        PropertyEditor() { }

        static String toString (const Identifier& name, const var& prop)
        {
            if (prop.isBool())          return booleanToString (static_cast<bool> (prop)).toLowerCase();
            if (prop.isVoid())          return "(void)";
            if (prop.isMethod())        return "(method)";
            if (prop.isObject())        return "(object)";
            if (prop.isBinaryData())    return "(binary data)";
            if (prop.isArray())         return "(array)";

            if (name.toString().contains ("colour") || name.toString().contains ("color"))
                return VariantConverter<Colour>::fromVar (prop).toDisplayString (true);

            return prop.toString();
        }

        void setSource (ValueTree& newSource)
        {
            clear();

            tree = newSource;

            constexpr int maxChars = 200;
            Array<PropertyComponent*> pc;

            for (int i = 0; i < tree.getNumProperties(); ++i)
            {
                const auto name = tree.getPropertyName (i);
                const auto value = tree.getPropertyAsValue (name, nullptr);

                PropertyComponent* tpc = nullptr;

                const auto v = value.getValue();
                if (v.isObject())
                {
                    tpc = new TextPropertyComponent (noEditValue, name.toString(), maxChars, false, false);
                    tpc->setEnabled (false);
                }
                else if (v.isBool())
                {
                    tpc = new BooleanPropertyComponent (value, name.toString(), String());
                }
                else if (name.toString().contains ("colour") || name.toString().contains ("color"))
                {
                    tpc = new ColourPropertyComponent (value, name.toString());
                }
                else
                {
                    tpc = new TextPropertyComponent (value, name.toString(), maxChars, false);
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
        Item (PropertyEditor& pe, const ValueTree& sourceTree) :
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
                    addSubItem (new Item (propertiesEditor, child));
        }

        void paintItem (Graphics& g, int w, int h) override
        {
            if (isSelected())
                g.fillAll (Colours::white);

            const Font font;

            auto typeName = tree.getType().toString();
            if (tree.getNumChildren() > 0)
                typeName << " , numChildren: " << tree.getNumChildren();

            const auto nameWidth = font.getStringWidthFloat (typeName);
            constexpr auto padding = 20.0f;
            const auto propertyX = padding + nameWidth;

            g.setColour (Colours::black);
            g.setFont (font);
            g.drawText (typeName, 0, 0, w, h, Justification::centredLeft, false);

            g.setColour (Colours::blue);
            g.setFont (font.withHeight (13.0f));

            String propertySummary;
            const int numProps = tree.getNumProperties();
            for (int i = 0; i < numProps; ++i)
            {
                const auto name = tree.getPropertyName (i);
                const auto& prop = tree[name];

                propertySummary << " [" << name.toString() << "] " << PropertyEditor::toString (name, prop);
            }

            g.drawText (propertySummary,
                        juce::Rectangle<float> (propertyX, 0.0f, (float) w - propertyX, (float) h),
                        Justification::centredLeft, true);
        }

        void itemSelectionChanged (bool isNowSelected) override
        {
            if (isNowSelected)
            {
                tree.removeListener (this);
                propertiesEditor.setSource (tree);
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
        PropertyEditor& propertiesEditor;

        ValueTree tree;
        Array<Identifier> currentProperties;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Item)
    };

    //==============================================================================
    ValueTree tree;
    TreeView treeView;
    PropertyEditor propertyEditor;
    StretchableLayoutManager layout;
    StretchableLayoutResizerBar layoutResizer;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValueTreeEditor)
};
