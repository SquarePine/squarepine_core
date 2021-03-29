/** Formats a provided JSON string into something that will hopefully be readable. */
void beautifyJSON (String& source,
                   int numSpaces = 4,
                   bool bracesAndBracketsOnNewline = true,
                   bool replaceTabsWithSpaces = true);
