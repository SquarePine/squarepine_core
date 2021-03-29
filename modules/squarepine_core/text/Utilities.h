/** @returns a string representing a given price, formatted as per the user's locale.
*/
inline String createLocaleFormattedPrice (double value)
{
    value *= 100.0; //std::put_money operates on a cents basis. https://stackoverflow.com/a/50476453/1907103

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
