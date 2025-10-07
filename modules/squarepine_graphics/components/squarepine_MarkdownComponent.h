/** A simple but functional Markdown parser and renderer component for JUCE.

    This component can parse and render basic Markdown syntax including:
    - Headers (# ## ###)
    - Bold (**text**)
    - Italic (*text*)
    - Code blocks (```code```)
    - Inline code (`code`)
    - Lists (- item, 1. item)
    - Links ([text](url))
    - Paragraphs
    - Line breaks

    Usage:
    @code
    MarkdownComponent markdown;
    markdown.setMarkdownText ("# Hello\n\nThis is **bold** and *italic* text.");
    // or
    markdown.setMarkdownFile (File ("README.md"));
    @endcode
*/
class MarkdownComponent final : public Component,
                                private Timer
{
public:
    //==============================================================================
    /** Constructor. */
    MarkdownComponent();

    /** Destructor. */
    ~MarkdownComponent() override;

    //==============================================================================
    /** Sets the markdown content from a string. */
    void setMarkdownText (const String&);

    /** Sets the markdown content from a file. */
    void setMarkdownFile (const File&);

    /** @returns the current markdown text. */
    const String& getMarkdownText() const noexcept      { return rawMarkdownText; }

    /** @returns the current file being displayed (if any). */
    const File& getMarkdownFile() const noexcept        { return currentFile; }

    //==============================================================================
    /** Sets whether to automatically reload the file when it changes. */
    void setAutoReload (bool shouldAutoReload);

    /** @returns true if auto-reload is enabled. */
    constexpr bool isAutoReloadEnabled() const noexcept { return autoReload; }

    //==============================================================================
    /** Sets the base font size for the markdown content. */
    void setBaseFontSize (float newSize);

    /** @returns the base font size. */
    constexpr float getBaseFontSize() const noexcept    { return baseFontSize; }

    /** @returns the total height of the rendered content in pixels. */
    float getTotalContentHeight() const noexcept        { return totalContentHeight; }

    /** Sets the preferred width for layout calculations. */
    void setPreferredWidth (int width);

    /** Sets the colour scheme for the markdown content. */
    void setColours (Colour textColour, Colour headingColour, Colour codeColour, Colour linkColour);

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override;
    /** @internal */
    void resized() override;
    /** @internal */
    void mouseDown (const MouseEvent& e) override;
    /** @internal */
    void lookAndFeelChanged() override;

private:
    //==============================================================================
    /** Table model for rendering markdown tables. */
    class MarkdownTableModel final : public TableListBoxModel
    {
    public:
        MarkdownTableModel (const StringArray& headers, const Array<StringArray>& rows, const MarkdownComponent& parent);

        int getNumRows() override;
        void paintRowBackground (Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;
        void paintCell (Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    private:
        StringArray tableHeaders;
        Array<StringArray> tableRows;
        const MarkdownComponent& parentComponent;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MarkdownTableModel)
    };

    //==============================================================================
    /** Represents a parsed markdown element. */
    struct MarkdownElement
    {
        enum class Type
        {
            Heading1,
            Heading2,
            Heading3,
            Heading4,
            Heading5,
            Heading6,
            Paragraph,
            Bold,
            Italic,
            Code,
            InlineCode,
            List,
            OrderedList,
            Link,
            CodeBlock,
            Table,
            Image,
            LineBreak
        };

        Type type = Type::Paragraph;
        String content,
               url;                                     // For links
        int level = 0;                                  // For headings and lists
        OwnedArray<MarkdownElement> children;
        juce::Rectangle<float> bounds;

        // Table-specific data
        StringArray tableHeaders;
        Array<StringArray> tableRows;
        std::unique_ptr<Component> tableComponent;      // Will hold the TableListBox
        std::unique_ptr<MarkdownTableModel> tableModel; // Owns the table model

        // Image-specific data
        std::unique_ptr<Drawable> drawable; // The loaded image/vector graphic
        String altText;                     // Alternative text for accessibility
        std::optional<int> maxWidth,        // Maximum width
                           maxHeight;       // Maximum height

        MarkdownElement (Type t = Type::Paragraph) noexcept : type (t) {}
    };

    //==============================================================================
    String rawMarkdownText;
    File currentFile;
    Time lastFileModTime;
    bool autoReload = false;
    File baseDirectory;                             // For resolving relative image paths
    OwnedArray<MarkdownElement> parsedElements;
    float baseFontSize = 14.0f,
          totalContentHeight = 0.0f;
    int preferredWidth = 0;

    // Colours
    Colour textColour = Colours::black;
    Colour headingColour = Colours::darkblue;
    Colour codeColour = Colours::darkgreen;
    Colour linkColour = Colours::blue;

    //==============================================================================
    /** Parses the markdown text into structured elements. */
    void parseMarkdown();

    /** Parses a single line of markdown. */
    MarkdownElement* parseLine (const String&);

    /** Parses inline markdown elements within text. */
    void parseInlineElements (const String&, OwnedArray<MarkdownElement>&);

    /** Calculates layout for all elements. */
    void calculateLayout();

    /** Calculates layout for a single element. */
    float layoutElement (MarkdownElement&, float x, float y, float width);

    /** Renders a single element. */
    void renderElement (Graphics& g, const MarkdownElement&);

    /** Gets the font for a specific element type. */
    Font getFontForElement (const MarkdownElement&) const;

    /** Gets the colour for a specific element type. */
    Colour getColourForElement (const MarkdownElement&) const;

    /** Finds the element at a given point (for link clicking). */
    const MarkdownElement* findElementAt (Point<float> position) const;

    /** Recursively searches for element at position. */
    const MarkdownElement* findElementAtRecursive (const MarkdownElement&, Point<float> position) const;

    /** Parses markdown table syntax and creates table elements. */
    MarkdownElement* parseTable (const StringArray& lines, int& currentLineIndex);

    /** Creates a TableListBox component for a table element. */
    void createTableComponent (MarkdownElement&);

    /** Loads an image from a URL or file path. */
    std::unique_ptr<Drawable> loadImage (const String& imageUrl, const File& baseDirectory = {});

    /** Parses image size specifications (e.g., "300x200", "300", "x200"). */
    void parseImageSize (const String& sizeSpec, int& maxWidth, int& maxHeight);

    //==============================================================================
    /** @internal */
    void timerCallback() override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MarkdownComponent)
};
