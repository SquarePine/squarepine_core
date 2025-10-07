//==============================================================================
MarkdownComponent::MarkdownTableModel::MarkdownTableModel (const StringArray& headers,
                                                           const Array<StringArray>& rows,
                                                           const MarkdownComponent& parent) :
    tableHeaders (headers),
    tableRows (rows),
    parentComponent (parent)
{
}

int MarkdownComponent::MarkdownTableModel::getNumRows()
{
    return tableRows.size();
}

void MarkdownComponent::MarkdownTableModel::paintRowBackground (Graphics& g, int rowNumber, int, int, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (parentComponent.findColour (TextEditor::highlightColourId));
    else if (isEven (rowNumber))
        g.fillAll (parentComponent.findColour (ListBox::backgroundColourId).contrasting (0.05f));
}

void MarkdownComponent::MarkdownTableModel::paintCell (Graphics& g, int rowNumber, int columnId, int width, int height, bool)
{
    if (isPositiveAndBelow (rowNumber, tableRows.size()))
    {
        const auto& row = tableRows.getReference (rowNumber);
        if (columnId > 0 && columnId <= row.size())
        {
            const auto cellText = row[columnId - 1]; // columnId is 1-based

            g.setColour (parentComponent.findColour (Label::textColourId));
            g.setFont (parentComponent.getFontForElement (MarkdownComponent::MarkdownElement (MarkdownComponent::MarkdownElement::Type::Paragraph)));

            const auto textArea = juce::Rectangle<int> (2, 0, width - 4, height);
            g.drawText (cellText, textArea, Justification::centredLeft, true);
        }
    }
}

//==============================================================================
MarkdownComponent::MarkdownComponent()
{
    setOpaque (false);
    lookAndFeelChanged();
}

MarkdownComponent::~MarkdownComponent()
{
    stopTimer();
}

//==============================================================================
void MarkdownComponent::setMarkdownText (const String& markdownText)
{
    if (rawMarkdownText != markdownText)
    {
        rawMarkdownText = markdownText;
        currentFile = File();
        baseDirectory = File::getCurrentWorkingDirectory(); // Default to current working directory
        parseMarkdown();
        repaint();
    }
}

void MarkdownComponent::setMarkdownFile (const File& markdownFile)
{
    if (markdownFile != currentFile && markdownFile.existsAsFile())
    {
        currentFile = markdownFile;
        baseDirectory = markdownFile.getParentDirectory(); // Set base directory for relative image paths
        lastFileModTime = markdownFile.getLastModificationTime();
        rawMarkdownText = markdownFile.loadFileAsString();
        parseMarkdown();
        repaint();

        if (autoReload)
            startTimer (1000); // Check for file changes every second
    }
}

//==============================================================================
void MarkdownComponent::setAutoReload (bool shouldAutoReload)
{
    autoReload = shouldAutoReload;

    if (autoReload && currentFile.existsAsFile())
        startTimerHz (1);
    else
        stopTimer();
}

//==============================================================================
void MarkdownComponent::setBaseFontSize (float newSize)
{
    if (! exactlyEqual (baseFontSize, newSize))
    {
        baseFontSize = newSize;
        calculateLayout();
        repaint();
    }
}

void MarkdownComponent::setColours (Colour textCol, Colour headingCol, Colour codeCol, Colour linkCol)
{
    textColour = textCol;
    headingColour = headingCol;
    codeColour = codeCol;
    linkColour = linkCol;
    repaint();
}

void MarkdownComponent::setPreferredWidth (int width)
{
    if (preferredWidth != width)
    {
        preferredWidth = width;
        calculateLayout();
        repaint();
    }
}

//==============================================================================
void MarkdownComponent::paint (Graphics& g)
{
    g.fillAll (findColour (ResizableWindow::backgroundColourId));

    for (const auto* element : parsedElements)
        renderElement (g, *element);
}

void MarkdownComponent::resized()
{
    calculateLayout();
}

void MarkdownComponent::mouseDown (const MouseEvent& e)
{
    if (const auto* element = findElementAt (e.position))
        if (element->type == MarkdownElement::Type::Link && element->url.isNotEmpty())
            URL (element->url).launchInDefaultBrowser();
}

void MarkdownComponent::lookAndFeelChanged()
{
    auto& lf = getLookAndFeel();
    textColour      = lf.findColour (Label::textColourId);
    headingColour   = lf.findColour (Label::textColourId).darker (0.3f);
    codeColour      = lf.findColour (Label::textColourId).withHue (0.3f);
    linkColour      = lf.findColour (HyperlinkButton::textColourId);
    repaint();
}

//==============================================================================
void MarkdownComponent::parseMarkdown()
{
    parsedElements.clear();

    auto lines = StringArray::fromLines (rawMarkdownText);
    bool inCodeBlock = false;
    String codeBlockContent;

    for (int i = 0; i < lines.size(); ++i)
    {
        auto trimmedLine = lines[i].trim();

        // Handle code blocks
        if (trimmedLine.startsWith ("```"))
        {
            if (inCodeBlock)
            {
                // End code block
                auto codeElement = new MarkdownElement (MarkdownElement::Type::CodeBlock);
                codeElement->content = codeBlockContent;
                parsedElements.add (codeElement);
                codeBlockContent.clear();
                inCodeBlock = false;
            }
            else
            {
                // Start code block
                inCodeBlock = true;
            }
            continue;
        }

        if (inCodeBlock)
        {
            codeBlockContent += lines[i] + "\n";
            continue;
        }

        // Check for table (line contains |)
        if (trimmedLine.contains ("|") && i + 1 < lines.size())
        {
            auto nextLine = lines[i + 1].trim();
            // Check if next line is a separator (contains | and -)
            if (nextLine.contains ("|") && nextLine.contains ("-"))
            {
                if (auto tableElement = parseTable (lines, i))
                {
                    parsedElements.add (tableElement);
                    createTableComponent (*tableElement);
                    continue; // i is already advanced by parseTable
                }
            }
        }

        // Parse regular lines
        if (auto element = parseLine (lines[i]))
            parsedElements.add (element);
    }

    calculateLayout();
}

MarkdownComponent::MarkdownElement* MarkdownComponent::parseLine (const String& line)
{
    auto trimmed = line.trimStart();

    if (trimmed.isEmpty())
        return new MarkdownElement (MarkdownElement::Type::LineBreak);

    // Headers:
    auto parseHeader = [&] (MarkdownElement::Type t, StringRef hashPattern) -> MarkdownElement*
    {
        if (trimmed.startsWith (hashPattern))
        {
            auto element = new MarkdownElement (t);
            element->content = trimmed.substring (static_cast<int> (t) + 1);
            return element;
        }

        return nullptr;
    };

    if (auto* me = parseHeader (MarkdownElement::Type::Heading1, "# "))         return me;
    if (auto* me = parseHeader (MarkdownElement::Type::Heading2, "## "))        return me;
    if (auto* me = parseHeader (MarkdownElement::Type::Heading3, "### "))       return me;
    if (auto* me = parseHeader (MarkdownElement::Type::Heading4, "#### "))      return me;
    if (auto* me = parseHeader (MarkdownElement::Type::Heading5, "##### "))     return me;
    if (auto* me = parseHeader (MarkdownElement::Type::Heading6, "####### "))   return me;

    // Lists
    if (trimmed.startsWith ("- ") || trimmed.startsWith ("* "))
    {
        auto element = new MarkdownElement (MarkdownElement::Type::List);
        element->content = trimmed.substring (2);
        parseInlineElements (element->content, element->children);
        return element;
    }

    // Ordered lists
    if (trimmed.matchesWildcard ("?. *", true))
    {
        auto element = new MarkdownElement (MarkdownElement::Type::OrderedList);
        auto dotPos = trimmed.indexOfChar ('.');
        element->content = trimmed.substring (dotPos + 2);
        parseInlineElements (element->content, element->children);
        return element;
    }

    // Regular paragraph
    auto element = new MarkdownElement (MarkdownElement::Type::Paragraph);
    element->content = trimmed;
    parseInlineElements (element->content, element->children);
    return element;
}

void MarkdownComponent::parseInlineElements (const String& text, juce::OwnedArray<MarkdownElement>& elements)
{
    String remaining = text;

    while (remaining.isNotEmpty())
    {
        // Find the next markdown syntax
        auto boldPos    = remaining.indexOf ("**");
        auto italicPos  = remaining.indexOf ("*");
        auto codePos    = remaining.indexOf ("`");
        auto linkPos    = remaining.indexOf ("[");
        auto imagePos   = remaining.indexOf ("![");

        // Adjust italic position if it's part of bold
        if (italicPos >= 0 && boldPos >= 0 && italicPos == boldPos)
            italicPos = remaining.indexOfChar (boldPos + 2, '*');

        int nextPos = -1;
        auto nextType = MarkdownElement::Type::Paragraph;

        // Find the earliest syntax (check images before links since ![...] starts with !)
        if (imagePos >= 0 && (nextPos < 0 || imagePos < nextPos))
        {
            nextPos = imagePos;
            nextType = MarkdownElement::Type::Image;
        }
        if (boldPos >= 0 && (nextPos < 0 || boldPos < nextPos))
        {
            nextPos = boldPos;
            nextType = MarkdownElement::Type::Bold;
        }
        if (italicPos >= 0 && (nextPos < 0 || italicPos < nextPos))
        {
            nextPos = italicPos;
            nextType = MarkdownElement::Type::Italic;
        }
        if (codePos >= 0 && (nextPos < 0 || codePos < nextPos))
        {
            nextPos = codePos;
            nextType = MarkdownElement::Type::InlineCode;
        }
        if (linkPos >= 0 && (nextPos < 0 || linkPos < nextPos))
        {
            nextPos = linkPos;
            nextType = MarkdownElement::Type::Link;
        }

        if (nextPos < 0)
        {
            // No more syntax, add remaining text as paragraph
            if (remaining.isNotEmpty())
            {
                auto element = new MarkdownElement (MarkdownElement::Type::Paragraph);
                element->content = remaining;
                elements.add (element);
            }

            break;
        }

        // Add text before the syntax
        if (nextPos > 0)
        {
            auto element = new MarkdownElement (MarkdownElement::Type::Paragraph);
            element->content = remaining.substring (0, nextPos);
            elements.add (element);
        }

        // Handle the specific syntax
        if (nextType == MarkdownElement::Type::Bold)
        {
            auto searchText = remaining.substring (nextPos + 2);
            auto endPos = searchText.indexOf ("**");
            if (endPos >= 0)
            {
                auto element = new MarkdownElement (MarkdownElement::Type::Bold);
                element->content = searchText.substring (0, endPos);
                elements.add (element);
                remaining = remaining.substring (nextPos + 4 + endPos);
            }
            else
            {
                remaining = remaining.substring (nextPos + 2);
            }
        }
        else if (nextType == MarkdownElement::Type::Italic)
        {
            auto searchText = remaining.substring (nextPos + 1);
            auto endPos = searchText.indexOfChar ('*');
            if (endPos >= 0)
            {
                auto element = new MarkdownElement (MarkdownElement::Type::Italic);
                element->content = searchText.substring (0, endPos);
                elements.add (element);
                remaining = remaining.substring (nextPos + 2 + endPos);
            }
            else
            {
                remaining = remaining.substring (nextPos + 1);
            }
        }
        else if (nextType == MarkdownElement::Type::InlineCode)
        {
            auto searchText = remaining.substring (nextPos + 1);
            auto endPos = searchText.indexOfChar ('`');
            if (endPos >= 0)
            {
                auto element = new MarkdownElement (MarkdownElement::Type::InlineCode);
                element->content = searchText.substring (0, endPos);
                elements.add (element);
                remaining = remaining.substring (nextPos + 2 + endPos);
            }
            else
            {
                remaining = remaining.substring (nextPos + 1);
            }
        }
        else if (nextType == MarkdownElement::Type::Image)
        {
            auto searchText = remaining.substring (nextPos);
            auto closeBracket = searchText.indexOfChar (']');

            if (closeBracket > 2) // Must have at least "![" + some content + "]"
            {
                auto afterBracket = searchText.substring (closeBracket + 1);
                if (afterBracket.startsWith ("("))
                {
                    auto closeParen = afterBracket.indexOfChar (')');
                    if (closeParen > 0)
                    {
                        auto element = new MarkdownElement (MarkdownElement::Type::Image);
                        element->altText = searchText.substring (2, closeBracket); // Skip "!["

                        // Parse URL and optional size specification
                        auto imageSpec = afterBracket.substring (1, closeParen);
                        auto spacePos = imageSpec.indexOfChar (' ');

                        if (spacePos > 0)
                        {
                            // Format: ![alt](image.png "title" 300x200)
                            element->url = imageSpec.substring (0, spacePos);
                            auto sizeSpec = imageSpec.substring (spacePos + 1).trim();

                            // Remove optional title in quotes
                            if (sizeSpec.startsWith ("\""))
                            {
                                auto endQuote = sizeSpec.indexOfChar (1, '"');
                                if (endQuote > 0)
                                    sizeSpec = sizeSpec.substring (endQuote + 1).trim();
                            }

                            int w = -1, h = -1;
                            parseImageSize (sizeSpec, w, h);

                            if (w > 0) element->maxWidth = w;
                            if (h > 0) element->maxHeight = h;
                        }
                        else
                        {
                            element->url = imageSpec;
                        }

                        // Load the image
                        element->drawable = loadImage (element->url, baseDirectory);

                        elements.add (element);
                        remaining = remaining.substring (nextPos + closeBracket + closeParen + 3);
                    }
                    else
                    {
                        remaining = remaining.substring (nextPos + 2);
                    }
                }
                else
                {
                    remaining = remaining.substring (nextPos + 2);
                }
            }
            else
            {
                remaining = remaining.substring (nextPos + 2);
            }
        }
        else if (nextType == MarkdownElement::Type::Link)
        {
            const auto searchText = remaining.substring (nextPos);
            const auto closeBracket = searchText.indexOfChar (']');

            if (closeBracket > 0)
            {
                const auto afterBracket = searchText.substring (closeBracket + 1);
                if (afterBracket.startsWith ("("))
                {
                    const auto closeParen = afterBracket.indexOfChar (')');
                    if (closeParen > 0)
                    {
                        auto element = new MarkdownElement (MarkdownElement::Type::Link);
                        element->content = searchText.substring (1, closeBracket);
                        element->url = afterBracket.substring (1, closeParen);
                        elements.add (element);
                        remaining = remaining.substring (nextPos + closeBracket + closeParen + 2);
                        continue; // Skip the fallback substring operation
                    }
                }
            }

            // Only advance if no valid link was found
            remaining = remaining.substring (nextPos);
        }
    }
}

//==============================================================================
void MarkdownComponent::calculateLayout()
{
    totalContentHeight = 0.0f;
    auto y = 10.0f;
    auto width = preferredWidth > 0 ? (float) preferredWidth - 20.0f : (float) getWidth() - 20.0f;

    if (width <= 0.0f)
        return;

    for (auto* element : parsedElements)
        y = layoutElement (*element, 10.0f, y, width);

    totalContentHeight = y + 10.0f; // Add bottom padding

    // Update component size to match content when using preferred width
    if (preferredWidth > 0)
        setSize (preferredWidth, jmax (100, (int) totalContentHeight));
}

float MarkdownComponent::layoutElement (MarkdownElement& element, float x, float y, float width)
{
    auto font = getFontForElement (element);

    if (element.type == MarkdownElement::Type::LineBreak)
    {
        element.bounds = { x, y, width, font.getHeight() * 0.5f };
        return y + element.bounds.getHeight();
    }

    if (element.type == MarkdownElement::Type::CodeBlock)
    {
        auto height = (float) StringArray::fromLines (element.content).size() * font.getHeight() + 10.0f;
        element.bounds = { x, y, width, height };
        return y + height + 10.0f;
    }

    if (element.type == MarkdownElement::Type::Table && element.tableComponent != nullptr)
    {
        auto tableHeight = (float) element.tableComponent->getHeight();
        element.bounds = { x, y, width, tableHeight };
        element.tableComponent->setBounds (element.bounds.toNearestInt());
        element.tableComponent->setVisible (true);
        return y + tableHeight + 10.0f;
    }

    if (element.type == MarkdownElement::Type::Image)
    {
        float imageWidth = 1.0f, 
              imageHeight = 1.0f;

        if (element.drawable != nullptr)
        {
        }

#if 0
        auto drawableBounds = element.drawable->getDrawableBounds();
        auto imageWidth = drawableBounds.getWidth();
        auto imageHeight = drawableBounds.getHeight();

        // Apply size constraints if specified
        if (element.maxWidth > 0)
            imageWidth = jmin (imageWidth, (float) element.maxWidth);
        if (element.maxHeight > 0)
            imageHeight = jmin (imageHeight, (float) element.maxHeight);

        // Maintain aspect ratio
        if (element.maxWidth > 0 && element.maxHeight > 0)
        {
            auto aspectRatio = drawableBounds.getWidth() / drawableBounds.getHeight();
            auto constrainedWidth = (float) element.maxWidth;
            auto constrainedHeight = (float) element.maxHeight;

            if (constrainedWidth / constrainedHeight > aspectRatio)
                constrainedWidth = constrainedHeight * aspectRatio;
            else
                constrainedHeight = constrainedWidth / aspectRatio;

            imageWidth = constrainedWidth;
            imageHeight = constrainedHeight;
        }

        // Constrain to available width
        if (imageWidth > width)
        {
            auto scale = width / imageWidth;
            imageWidth = width;
            imageHeight *= scale;
        }
#endif

        element.bounds = { x, y, imageWidth, imageHeight };
        return y + imageHeight + 10.0f;
    }

    // For elements with children, layout inline
    if (element.children.size() > 0)
    {
        auto currentX = x;
        auto lineHeight = font.getHeight();
        auto startY = y;

        for (auto* child : element.children)
        {
            auto childFont = getFontForElement (*child);
            GlyphArrangement glyphs;
            glyphs.addLineOfText (childFont, child->content, 0.0f, 0.0f);
            auto textWidth = glyphs.getBoundingBox (0, -1, true).getWidth();

            if (currentX + textWidth > x + width && currentX > x)
            {
                // Wrap to next line
                y += lineHeight;
                currentX = x;
            }

            child->bounds = { currentX, y, textWidth, childFont.getHeight() };
            currentX += textWidth;
            lineHeight = jmax (lineHeight, childFont.getHeight());
        }

        element.bounds = { x, startY, width, y + lineHeight - startY };
        return y + lineHeight + 5.0f;
    }

    // Simple single-line elements
    auto height = font.getHeight();
    element.bounds = { x, y, width, height };

    // Add extra spacing for headings
    if (element.type >= MarkdownElement::Type::Heading1 && element.type <= MarkdownElement::Type::Heading3)
        return y + height + 10.0f;

    return y + height + 5.0f;
}

//==============================================================================
void MarkdownComponent::renderElement (Graphics& g, const MarkdownElement& element)
{
    if (element.type == MarkdownElement::Type::LineBreak)
        return;

    if (element.type == MarkdownElement::Type::Table)
        return;

    auto font = getFontForElement (element);
    auto colour = getColourForElement (element);

    g.setFont (font);
    g.setColour (colour);

    if (element.type == MarkdownElement::Type::CodeBlock)
    {
        // Draw code block background
        g.setColour (colour.withAlpha (0.1f));
        g.fillRoundedRectangle (element.bounds, 4.0f);

        g.setColour (colour);
        g.drawRoundedRectangle (element.bounds, 4.0f, 1.0f);

        // Draw code text
        auto codeArea = element.bounds.reduced (5.0f);
        g.drawText (element.content, codeArea, Justification::topLeft);
        return;
    }

    // Draw the image if required
    if (element.type == MarkdownElement::Type::Image
        && element.drawable != nullptr)
    {
        element.drawable->drawWithin (g, element.bounds, RectanglePlacement::centred | RectanglePlacement::stretchToFit, 1.0f);
        return;
    }

    // Render children inline
    if (! element.children.isEmpty())
    {
        for (const auto* child : element.children)
            renderElement (g, *child);

        return;
    }

    // Render single element
    String prefix;
    if (element.type == MarkdownElement::Type::List)
        prefix = "• ";
    else if (element.type == MarkdownElement::Type::OrderedList)
        prefix = "1. ";

    g.drawText (prefix + element.content, element.bounds, Justification::centredLeft);

    // Underline links
    if (element.type == MarkdownElement::Type::Link)
    {
        const auto y = element.bounds.getBottom() - 1.0f;
        g.drawLine (element.bounds.getX(), y, element.bounds.getRight(), y, 1.0f);
    }
}

//==============================================================================
Font MarkdownComponent::getFontForElement (const MarkdownElement& element) const
{
    switch (element.type)
    {
        case MarkdownElement::Type::Heading1:   return Font (FontOptions (baseFontSize * 1.9f)).withStyle (Font::bold);
        case MarkdownElement::Type::Heading2:   return Font (FontOptions (baseFontSize * 1.7f)).withStyle (Font::bold);
        case MarkdownElement::Type::Heading3:   return Font (FontOptions (baseFontSize * 1.5f)).withStyle (Font::bold);
        case MarkdownElement::Type::Heading4:   return Font (FontOptions (baseFontSize * 1.3f)).withStyle (Font::bold);
        case MarkdownElement::Type::Heading5:   return Font (FontOptions (baseFontSize * 1.1f)).withStyle (Font::bold);
        case MarkdownElement::Type::Heading6:   return Font (FontOptions (baseFontSize * 0.8f)).withStyle (Font::bold);
        case MarkdownElement::Type::Bold:       return Font (FontOptions (baseFontSize)).withStyle (Font::bold);
        case MarkdownElement::Type::Italic:     return Font (FontOptions (baseFontSize)).withStyle (Font::italic);

        case MarkdownElement::Type::Code:
        case MarkdownElement::Type::InlineCode:
        case MarkdownElement::Type::CodeBlock:
            return Font (FontOptions (Font::getDefaultMonospacedFontName(), baseFontSize, Font::plain));

        default:
            return Font (FontOptions (baseFontSize));
    }
}

Colour MarkdownComponent::getColourForElement (const MarkdownElement& element) const
{
    switch (element.type)
    {
        case MarkdownElement::Type::Heading1:
        case MarkdownElement::Type::Heading2:
        case MarkdownElement::Type::Heading3:
        case MarkdownElement::Type::Heading4:
        case MarkdownElement::Type::Heading5:
        case MarkdownElement::Type::Heading6:
            return headingColour;

        case MarkdownElement::Type::Code:
        case MarkdownElement::Type::InlineCode:
        case MarkdownElement::Type::CodeBlock:
            return codeColour;

        case MarkdownElement::Type::Link:
            return linkColour;

        default:
            return textColour;
    }
}

//==============================================================================
const MarkdownComponent::MarkdownElement* MarkdownComponent::findElementAt (juce::Point<float> position) const
{
    for (const auto* element : parsedElements)
        if (auto* found = findElementAtRecursive (*element, position))
            return found;

    return nullptr;
}

const MarkdownComponent::MarkdownElement* MarkdownComponent::findElementAtRecursive (const MarkdownElement& element, juce::Point<float> position) const
{
    if (element.bounds.contains (position))
    {
        for (const auto* child : element.children)
            if (auto* found = findElementAtRecursive (*child, position))
                return found;

        return &element;
    }

    return nullptr;
}

//==============================================================================
void MarkdownComponent::timerCallback()
{
    if (currentFile.existsAsFile())
    {
        const auto newModTime = currentFile.getLastModificationTime();
        if (newModTime != lastFileModTime)
        {
            lastFileModTime = newModTime;
            setMarkdownFile (currentFile);
        }
    }
}

//==============================================================================
MarkdownComponent::MarkdownElement* MarkdownComponent::parseTable (const StringArray& lines, int& currentLineIndex)
{
    auto tableElement = new MarkdownElement (MarkdownElement::Type::Table);

    // Parse header row
    auto headerLine = lines[currentLineIndex].trim();
    StringArray headers;
    auto headerCells = StringArray::fromTokens (headerLine, "|", "");
    for (auto& cell : headerCells)
    {
        auto trimmedCell = cell.trim();
        if (trimmedCell.isNotEmpty())
            headers.add (trimmedCell);
    }

    if (headers.isEmpty())
        return nullptr;

    tableElement->tableHeaders = headers;

    // Skip separator line
    currentLineIndex += 2;

    // Parse data rows
    while (currentLineIndex < lines.size())
    {
        auto rowLine = lines[currentLineIndex].trim();
        if (rowLine.isEmpty() || ! rowLine.contains ("|"))
            break;

        StringArray rowCells;
        for (auto& cell : StringArray::fromTokens (rowLine, "|", ""))
            if (auto trimmedCell = cell.trim(); trimmedCell.isNotEmpty())
                rowCells.add (trimmedCell);

        if (! rowCells.isEmpty())
            tableElement->tableRows.add (rowCells);

        ++currentLineIndex;
    }

    // Adjust index to point to last processed line
    --currentLineIndex;

    return tableElement;
}

void MarkdownComponent::createTableComponent (MarkdownElement& tableElement)
{
    auto tableListBox = std::make_unique<TableListBox>();
    tableElement.tableModel = std::make_unique<MarkdownTableModel> (tableElement.tableHeaders, tableElement.tableRows, *this);

    tableListBox->setModel (tableElement.tableModel.get()); // TableListBox does NOT take ownership
    tableListBox->setMultipleSelectionEnabled (false);
    tableListBox->setClickingTogglesRowSelection (false);

    // Set up columns
    auto& header = tableListBox->getHeader();
    for (int i = 0; i < tableElement.tableHeaders.size(); ++i)
    {
        header.addColumn (tableElement.tableHeaders[i],
                          i + 1,                                // columnId (1-based)
                          100,                                  // default width
                          50,                                   // minimum width
                          300,                                  // maximum width
                          TableHeaderComponent::defaultFlags);
    }

    header.setStretchToFitActive (true);

    // Calculate appropriate height (header + rows + padding)
    auto rowHeight = 25;
    auto headerHeight = 30;
    auto totalHeight = headerHeight + (tableElement.tableRows.size() * rowHeight) + 10;

    tableListBox->setSize (400, totalHeight); // Default width, calculated height

    tableElement.tableComponent = std::move (tableListBox);
    addChildComponent (tableElement.tableComponent.get());
}

//==============================================================================
std::unique_ptr<Drawable> MarkdownComponent::loadImage (const String& imageUrl, const File& baseDir)
{
    const URL url (imageUrl);

    // Check if it's likely a web/non-local URL:
    if (imageUrl.startsWithIgnoreCase ("http"))
    {
        // Try loading as a web image:
        if (auto inputStream = url.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)))
            if (const auto image = ImageFileFormat::loadFrom (*inputStream); image.isValid())
                return std::make_unique<DrawableImage> (image);

        // Try loading as a web SVG:
        if (auto inputStream = url.createInputStream (URL::InputStreamOptions (URL::ParameterHandling::inAddress)))
        {
            const auto svgData = inputStream->readEntireStreamAsString();
            if (svgData.isNotEmpty())
            {
                auto svg = Drawable::createFromImageData (svgData.toRawUTF8(), svgData.getNumBytesAsUTF8());
                if (svg != nullptr)
                    return svg;
            }
        }

        jassertfalse;
        return nullptr;
    }

    // Local file path attempts:
    File imageFile;

    if (File::isAbsolutePath (imageUrl))
        imageFile = File (imageUrl);
    else
        imageFile = baseDir.getChildFile (imageUrl);

    if (! imageFile.existsAsFile())
    {
        jassertfalse;
        return nullptr;
    }

    // Try to load as raster image:
    if (auto image = ImageFileFormat::loadFrom (imageFile); image.isValid())
        return std::make_unique<DrawableImage> (image);

    // Try to load as SVG:
    if (imageFile.hasFileExtension ("svg"))
        if (const auto svgText = imageFile.loadFileAsString(); svgText.isNotEmpty())
            if (auto svg = Drawable::createFromImageData (svgText.toRawUTF8(), svgText.getNumBytesAsUTF8()))
                return svg;

    return nullptr;
}

void MarkdownComponent::parseImageSize (const String& sizeSpec, int& maxWidth, int& maxHeight)
{
    maxWidth = 0;
    maxHeight = 0;

    const auto spec = sizeSpec.trim();
    if (spec.isEmpty())
        return;

    // Format: "300x200", "300", "x200", "300x"
    const auto xPos = spec.indexOfChar ('x');

    if (xPos > 0)
    {
        // Has width and possibly height
        maxWidth = spec.substring (0, xPos).getIntValue();

        if (xPos < spec.length() - 1)
            maxHeight = spec.substring (xPos + 1).getIntValue();
    }
    else if (xPos == 0)
    {
        // Format: "x200" (height only)
        maxHeight = spec.substring (1).getIntValue();
    }
    else
    {
        // No 'x', assume it's width only
        maxWidth = spec.getIntValue();
    }
}
