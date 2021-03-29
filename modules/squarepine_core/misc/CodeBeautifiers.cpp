void beautifyJSON (String& source, int numSpaces, bool bracesAndBracketsOnNewline, bool replaceTabsWithSpaces)
{
    source = source.trim();

    if (source.isEmpty())
        return;

    String result;
    result.preallocateBytes (String::CharPointerType::getBytesRequiredFor (source.getCharPointer()));

    auto cp = source.getCharPointer();

    const auto whitespace = String::repeatedString (" ", std::clamp (numSpaces, 0, 8));
    int level = 0;
    bool ignoreNext = false;
    bool inputString = false;

    if (replaceTabsWithSpaces && source.containsChar ('\t'))
        source = source.replaceCharacters ("\t", whitespace);

    auto appendWhitespace = [&]()
    {
        for (int i = level; --i >= 0;)
            result << whitespace;
    };

    auto c = cp.getAndAdvance();
    while (c != 0)
    {
        switch (c)
        {
            case 0:
            case '\r':
            case '\n':
            break;

            case '[':
            case '{':
                if (inputString)
                {
                    result << c;
                    break;
                }

                if (bracesAndBracketsOnNewline)
                {
                    result << newLine;
                    appendWhitespace();
                }

                result << c << newLine;
                ++level;

                appendWhitespace();
            break;

            case ']':
            case '}':
                if (inputString)
                {
                    result << c;
                    break;
                }

                if (level != 0)
                    --level;

                result << newLine;
                appendWhitespace();
                result << c;
            break;

            case ',':
                if (inputString)
                {
                    result << ",";
                    break;
                }

                result << "," << newLine;
                appendWhitespace();
            break;

            case '\\':
                ignoreNext = ! ignoreNext;

                result << "\\";
            break;

            case '"':
                if (! ignoreNext)
                    inputString = ! inputString;

                result << "\"";
            break;

            case ' ':
                if (inputString)
                    result << " ";
            break;

            case ':':
                result << ":";

                if (! inputString)
                    result << " ";
            break;

            default:
                ignoreNext = false;
                result << c;
            break;
        }

        c = cp.getAndAdvance();
    }

    result << newLine;
    source.swapWith (result);
}
