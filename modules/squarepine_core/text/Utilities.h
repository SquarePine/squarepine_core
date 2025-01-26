#pragma once

/** @returns a string representing a given price, formatted as per the user's locale.
*/
inline String createLocaleFormattedPrice (double value)
{
    value *= 100.0; // NB: std::put_money operates on a cents basis. https://stackoverflow.com/a/50476453/1907103

    const auto euroSymbol   = String::fromUTF8 ("\xe2\x82\xac");
    const auto yenSymbol    = String::fromUTF8 ("\xc2\xa5");
    const auto poundSymbol  = String::fromUTF8 ("\xc2\xa3");

    std::stringstream ss;
    ss.imbue (std::locale (""));
    ss << std::showbase << std::put_money ((long double) value);

    return String (ss.str())
        .replace ("$", euroSymbol, true)
        .replace (yenSymbol, euroSymbol, true)
        .replace (poundSymbol, euroSymbol, true);
}

//==============================================================================
/** @returns a string representing the value, formatted as per the user's locale.

    For the value "One Million & Ten Thousand One Hundred & Three and Two Thirds":
    - In en-CA (Canadian English):  1,010,103.6666666
    - In fr-CA (Canadian French):   1 010 103,6666666
*/
inline String createLocaleFormattedValue (double value)
{
    std::stringstream ss;
    ss.imbue (std::locale (""));
    ss << std::fixed << value;

    return String (ss.str());
}

/** @returns a string representing the value, formatted as per the user's locale.

    For the value "One Million & Ten Thousand One Hundred & Three and Two Thirds":
    - In en-CA (Canadian English):  1,010,103.6666666
    - In fr-CA (Canadian French):   1 010 103,6666666
*/
template<typename IntegralType>
inline String createLocaleFormattedValue (IntegralType value)
{
    std::stringstream ss;
    ss.imbue (std::locale (""));
    ss << std::fixed << value;

    return String (ss.str());
}

//==============================================================================
/** @returns a reversed version of the source string. */
inline String reverse (const String& source)
{
    auto start = source.getCharPointer();
    auto input = start.findTerminatingNull();

    String result;

    if (input != start)
    {
        result.preallocateBytes (static_cast<size_t> (input - start));

        auto output = result.getCharPointer();

        while (input != start)
        {
            --input;
            output.write (*input);
        }

        output.writeNull();
    }

    return result;
}

//==============================================================================
/** @returns true if the given string to look for is contained,
    as a substring, within the provided array of strings.

    @param source               The source list of strings to scan through.
    @param substringToLookFor   The substring to look for.
    @param caseSensitive        Whether this lookup cares about the string case or not. (By default this doesn't care.)
*/
inline bool containsSubstring (const StringArray& source, const String& substringToLookFor, bool caseSensitive = false)
{
    if (caseSensitive)
    {
        for (const auto& s : source)
            if (s.contains (substringToLookFor))
                return true;
    }
    else
    {
        for (const auto& s : source)
            if (s.containsIgnoreCase (substringToLookFor))
                return true;
    }

    return false;
}

//==============================================================================
/** @returns an upper-case version of the given character.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
juce_wchar toUpperCase (juce_wchar character) noexcept;

/** @returns a lower-case version of the given character.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
juce_wchar toLowerCase (juce_wchar character) noexcept;

/** @returns true if the given character is upper-case.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
bool isUpperCase (juce_wchar character) noexcept;

/** @returns true if the given character is lower-case.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
bool isLowerCase (juce_wchar character) noexcept;

/** @returns an upper-case version of the given juce::String.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
String toUpperCase (const String& source);

/** @returns a lower-case version of the given juce::String.

    JUCE's juce::String doesn't properly convert casing,
    what with it not supporting extended Latin codepoints
    and other languages, hence the need for this function.
*/
String toLowerCase (const String& source);
