//==============================================================================
/** Encompasses a font's range of typefaces
    for a particular weight (ie: normal, italic).
*/
class FontWeight final
{
public:
    /** Constructs a set of typefaces to outline a particular font weight.

        @param normalTypeface
        @param setAllToNormal If true, all typefaces will be set to the normal typeface.
    */
    FontWeight (Typeface::Ptr normalTypeface, bool setAllToNormal = true) :
        normal (normalTypeface)
    {
        jassert (normal != nullptr);

        if (setAllToNormal)
            thin = ultraLight = light = semiLight
                 = medium = semiBold = bold = extraBold
                 = black = extraBlack = ultraBlack = normal;
    }

    /** Constructs a set of typefaces to outline a particular font weight.

        @param normalTypefaceData
        @param normalTypefaceDataSize
        @param setAllToNormal If true, all typefaces will be set to the normal typeface.
    */
    FontWeight (const char* normalTypefaceData,
                int normalTypefaceDataSize,
                bool setAllToNormal = true) :
        FontWeight (Typeface::createSystemTypefaceFor (normalTypefaceData, normalTypefaceDataSize), setAllToNormal)
    {
    }

    //==============================================================================
    Typeface::Ptr thin, ultraLight, light, semiLight,
                  normal, medium, semiBold, bold, extraBold,
                  black, extraBlack, ultraBlack;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FontWeight)
};

//==============================================================================
/** Encompasses a font family consisting of the
    normal and italic types and their subsequent weights.
*/
class FontFamily final
{
public:
    /** Constructs a family of typefaces.

        @param familyName
        @param normalTypeface
        @param setAllToNormal If true, all typefaces will be set to the normal typeface.
    */
    FontFamily (const String& familyName,
                Typeface::Ptr normalTypeface,
                bool setAllToNormal = true) :
        name (familyName),
        regular (normalTypeface, setAllToNormal),
        italic (normalTypeface, setAllToNormal)
    {
        jassert (name.isNotEmpty());
    }

    /** Constructs a family of typefaces.

        @param familyName
        @param normalTypefaceData
        @param normalTypefaceDataSize
        @param setAllToNormal           If true, all typefaces will be set to the normal typeface.
    */
    FontFamily (const String& familyName,
                const char* normalTypefaceData,
                int normalTypefaceDataSize,
                bool setAllToNormal = true) :
        FontFamily (familyName, Typeface::createSystemTypefaceFor (normalTypefaceData, normalTypefaceDataSize), setAllToNormal)
    {
        jassert (name.isNotEmpty());
    }

    //==============================================================================
    /** @returns true if this font family is loaded with a normal typeface. */
    bool isValid() const { return regular.normal != nullptr; }

    /** @returns true if this font family is not loaded with a normal typeface. */
    bool isNull() const { return ! isValid(); }

    //==============================================================================
    const String name;
    FontWeight regular, italic;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FontFamily)
};

//==============================================================================
/** @returns the typeface for the specified typeface family, or nullptr on failure. */
inline Typeface::Ptr getTypefaceFromFamily (const Font& source, const String& nameToLookFor,
                                            Typeface::Ptr regular,
                                            Typeface::Ptr bold = nullptr,
                                            Typeface::Ptr italic = nullptr,
                                            Typeface::Ptr boldItalic = nullptr)
{
    if (nameToLookFor.equalsIgnoreCase (source.getTypefaceName()))
    {
        if (source.isBold() && bold != nullptr)
        {
            if (source.isItalic() && italic != nullptr)
                return boldItalic;

            return bold;
        }

        if (source.isItalic() && italic != nullptr)
            return italic;

        jassert (regular != nullptr);
        return regular;
    }

    return {};
}

/** @returns the typeface for the specified typeface family, or nullptr on failure. */
inline Typeface::Ptr getTypefaceFromFamily (const Font& source, const FontFamily& family)
{
    auto bestBold = family.regular.bold;
    if (bestBold == nullptr)
        bestBold = family.regular.semiBold;

    auto bestBoldItalic = family.italic.bold;
    if (bestBoldItalic == nullptr)
        bestBoldItalic = family.italic.semiBold;

    return getTypefaceFromFamily (source, family.name,
                                  family.regular.normal, bestBold,
                                  family.italic.normal, bestBoldItalic);
}
