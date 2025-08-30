//==============================================================================
/** Demo showing how to use the MarkdownComponent.

    This demonstrates parsing and rendering markdown from both strings and files,
    with auto-reload functionality and customizable styling.
*/
class MarkdownDemo final : public DemoBase,
                           private Button::Listener,
                           private FileDragAndDropTarget
{
public:
    /** Constructor. */
    MarkdownDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, "Markdown")
    {
        SQUAREPINE_CRASH_TRACER

        setSize (800, 600);

        // Create the markdown component and viewport
        viewport.setViewedComponent (&markdownComponent, false);
        viewport.setScrollBarsShown (true, false); // Vertical scrolling only
        addAndMakeVisible (viewport);

        // Create demo buttons
        loadStringButton.setButtonText (TRANS ("Load Sample Text"));
        loadStringButton.addListener (this);
        addAndMakeVisible (loadStringButton);

        loadFileButton.setButtonText (TRANS ("Load File..."));
        loadFileButton.addListener (this);
        addAndMakeVisible (loadFileButton);

        autoReloadToggle.setButtonText (TRANS ("Auto-reload file"));
        autoReloadToggle.setToggleable (true);
        autoReloadToggle.addListener (this);
        addAndMakeVisible (autoReloadToggle);

        fontSizeSlider.setSliderStyle (Slider::LinearHorizontal);
        fontSizeSlider.setRange (8.0, 24.0, 1.0);
        fontSizeSlider.setValue (14.0);
        fontSizeSlider.setTextBoxStyle (Slider::TextBoxRight, false, 50, 50);
        fontSizeSlider.onValueChange = [this]()
        {
            markdownComponent.setBaseFontSize ((float) fontSizeSlider.getValue());
            updateMarkdownSize();
        };

        addAndMakeVisible (fontSizeSlider);

        fontSizeLabel.setText (TRANS ("Font Size:"), dontSendNotification);
        fontSizeLabel.attachToComponent (&fontSizeSlider, true);
        addAndMakeVisible (fontSizeLabel);

        loadSampleMarkdown();
    }

    //==============================================================================
    /** @internal */
    void paint (Graphics& g) override
    {
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

        // Draw drag and drop hint
        if (isDragOver)
        {
            g.setColour (Colours::lightblue.withAlpha (0.3f));
            g.fillAll();

            g.setColour (Colours::lightblue);
            g.drawRect (getLocalBounds(), 2);

            g.setFont (16.0f);
            g.drawText (TRANS ("Drop a markdown file here!"), getLocalBounds(), Justification::centred);
        }
    }

    /** @internal */
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto buttonArea = bounds.removeFromTop (80).reduced (10);

        // First row of controls
        auto firstRow = buttonArea.removeFromTop (30);
        loadStringButton.setBounds (firstRow.removeFromLeft (120));
        firstRow.removeFromLeft (10);
        loadFileButton.setBounds (firstRow.removeFromLeft (120));
        firstRow.removeFromLeft (10);
        autoReloadToggle.setBounds (firstRow.removeFromLeft (120));

        // Second row for font size
        buttonArea.removeFromTop (10);
        auto secondRow = buttonArea.removeFromTop (30);
        secondRow.removeFromLeft (70); // Space for label
        fontSizeSlider.setBounds (secondRow.removeFromLeft (200));

        viewport.setBounds (bounds.reduced (10));
        updateMarkdownSize();
    }

    //==============================================================================
    /** @internal */
    bool isInterestedInFileDrag (const StringArray& files) override
    {
        for (const auto& file : files)
            if (File (file).hasFileExtension (".md;.markdown;.txt"))
                return true;

        return false;
    }

    /** @internal */
    void fileDragEnter (const StringArray&, int, int) override
    {
        isDragOver = true;
        repaint();
    }

    /** @internal */
    void fileDragExit (const StringArray&) override
    {
        isDragOver = false;
        repaint();
    }

    /** @internal */
    void filesDropped (const StringArray& files, int, int) override
    {
        isDragOver = false;

        if (const File file = files.strings.getFirst(); file.existsAsFile())
        {
            markdownComponent.setMarkdownFile (file);
            autoReloadToggle.setToggleState (false, dontSendNotification);
            updateMarkdownSize();
        }

        repaint();
    }

private:
    //==============================================================================
    Viewport viewport;
    sp::MarkdownComponent markdownComponent;
    TextButton loadStringButton, loadFileButton;
    ToggleButton autoReloadToggle;
    Slider fontSizeSlider;
    Label fontSizeLabel;
    std::unique_ptr<FileChooser> fileChooser;
    bool isDragOver = false;

    //==============================================================================
    void updateMarkdownSize()
    {
        if (viewport.getWidth() > 0)
            markdownComponent.setPreferredWidth (viewport.getWidth());
    }

    /** @internal */
    void buttonClicked (Button* button) override
    {
        SQUAREPINE_CRASH_TRACER

        if (button == &loadStringButton)
        {
            loadSampleMarkdown();
        }
        else if (button == &loadFileButton)
        {
            if (fileChooser != nullptr)
                return;

            fileChooser = std::make_unique<FileChooser> (TRANS ("Choose a markdown file..."), File(), "*.md;*.markdown;*.txt");

            fileChooser->launchAsync (FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
                                      [this] (const FileChooser& chooser)
                                      {
                                          if (const auto file = chooser.getResult(); file.existsAsFile())
                                          {
                                              markdownComponent.setMarkdownFile (file);
                                              autoReloadToggle.setToggleState (false, dontSendNotification);
                                              updateMarkdownSize();
                                          }
                                      });
        }
        else if (button == &autoReloadToggle)
        {
            markdownComponent.setAutoReload (autoReloadToggle.getToggleState());
        }
    }

    void loadSampleMarkdown()
    {
        const char* const sampleMarkdown = R"(# SquarePine Markdown Demo

This is a demonstration of the **MarkdownComponent** for JUCE.

## Features

The component supports many markdown features:

- **Bold text** using double asterisks
- *Italic text* using single asterisks
- `Inline code` using backticks
- [Links](https://www.squarepine.io) with clickable URLs
- Headers at multiple levels (from H1, H2, H3, H4, H5, and maximum H6)
- Unordered lists like this one
- Ordered lists (see below)
- Code blocks with syntax highlighting

### Images

The component supports both raster and vector images:

![Sample Image]({SAMPLE_SVG} 300x150)

*Example: Sample image demonstrating web URL loading*

You can also specify size constraints:
- `![alt text]({SAMPLE_PNG})` - Original size
- `![alt text]({SAMPLE_PNG} 300)` - Max width 300px
- `![alt text]({SAMPLE_PNG} x200)` - Max height 200px
- `![alt text]({SAMPLE_PNG} 300x200)` - Max 300x200px

**Local Images**: When loading from files, images are resolved relative to the markdown file's directory.

### Code Example

Here's some C++ code showing how to use the component:

```cpp
// Create and configure a MarkdownComponent
sp::MarkdownComponent markdown;
markdown.setMarkdownText ("# Hello World\n\nThis is **bold** text.");
markdown.setBaseFontSize (16.0f);
markdown.setAutoReload (true);

// Add to your component
addAndMakeVisible (markdown);
```

## Usage Instructions

1. **Load Sample Text**: Click to see this demo content
2. **Load File**: Browse for a `.md`, `.markdown`, or `.txt` file
3. **Auto-reload**: Enable to automatically reload files when they change
4. **Font Size**: Adjust the base font size with the slider
5. **Drag & Drop**: Drop markdown files directly onto this demo

### Customisation Options

The `MarkdownComponent` provides several customisation methods:

- `setBaseFontSize()` - Adjust the base font size
- `setColours()` - Customize text, heading, code, and link colors
- `setAutoReload()` - Enable/disable file change monitoring
- `setMarkdownText()` - Set content from a string
- `setMarkdownFile()` - Load content from a file

### Supported Markdown Syntax

| Element | Syntax | Example |
|---------|--------|---------|
| Headers | `# ## ###` | # Heading 1 |
| Bold | `**text**` | **bold text** |
| Italic | `*text*` | *italic text* |
| Code | `` `text` `` | `inline code` |
| Links | `[text](url)` | [SquarePine](https://squarepine.io) |
| Images | `![alt]({SAMPLE_PNG})` | ![Demo]({SAMPLE_PNG} 100x50) |
| Lists | `- item` or `1. item` | • Bullet or 1. Numbered |

That's it! The component handles parsing and rendering automatically with proper JUCE integration.

---

*Try dragging a markdown file onto this demo to see it in action!*
)";

        auto sm = String::fromUTF8 (sampleMarkdown)
                    .replace ("{SAMPLE_SVG}", "https://raw.githubusercontent.com/SquarePine/squarepine_core/main/demo/assets/logos/squarepine_logo_colour.svg")
                    .replace ("{SAMPLE_PNG}", "https://raw.githubusercontent.com/SquarePine/squarepine_core/main/demo/assets/logos/squarepine_icon_32.png");

        markdownComponent.setMarkdownText (sm);
        autoReloadToggle.setToggleState (false, dontSendNotification);
        updateMarkdownSize();
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MarkdownDemo)
};
