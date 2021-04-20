//==============================================================================
/** Encompasses a font's range of typefaces
    for a particular weight (ie: normal, italic).
*/
class FontWeight final
{
public:
    /** Constructs a set of null typefaces to outline a particular font weight. */
    FontWeight() = default;

    //==============================================================================
    /** It's strongly suggested to call this before laying out all of the typefaces! */
    void assignAllToNormal (Typeface::Ptr normalTypeface)
    {
        thin = ultraLight = light = semiLight =
        normal = medium = semiBold = bold = extraBold =
        black = extraBlack = ultraBlack = normalTypeface;
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
    //==============================================================================
    /** */
    FontFamily (const String& familyName) :
        name (familyName)
    {
        jassert (name.isNotEmpty());
    }

    //==============================================================================
    /** It's strongly suggested to call this before laying out all of the typefaces! */
    void assignAllToNormal (Typeface::Ptr normalTypeface)
    {
        regular.assignAllToNormal (normalTypeface);
        italic.assignAllToNormal (normalTypeface);
    }

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
