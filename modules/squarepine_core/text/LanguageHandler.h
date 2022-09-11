#pragma once

/** A basic representation of a standardised IETF language tags.

    Use one of these to indicate the language of text or
    other items in HTML documents, XML documents.
    Use the lang attribute to specify language tags in HTML,
    and the xml:lang attribute for XML.

    More importantly, this serves as a basis for managing languages,
    ie: translation files, in your application. You can do so via
    the combination of a IETFLanguageFile and a single instance of
    a LanguageHandler.

    @see IETFLanguageFile, LanguageHandler
*/
struct IETFLanguageTag final
{
    /** Constructs a default language tag of the Canadian English variety,
        represented as "eng-ca".
    */
    IETFLanguageTag() noexcept = default;

    /** */
    IETFLanguageTag (LanguageCode languageCode, CountryCode countryCode) noexcept :
        code (languageCode),
        country (countryCode)
    {
    }

    /** @returns true if this language tag matches the given other one. */
    bool operator== (const IETFLanguageTag& other) const noexcept
    {
        return code == other.code
            && country == other.country;
    }

    /** @returns true if this language tag does not match the given other one. */
    bool operator!= (const IETFLanguageTag& other) const noexcept { return ! operator== (other); }

    /** @returns a valid language tag if the string was understandible.

        Otherwise, the tag will be made up of an unknown language code
        and unknown contry code.
    */
    static IETFLanguageTag fromString (const String& s)
    {
        const auto toks = StringArray::fromTokens (s, "-", "");
        if (toks.size() != 2)
            return { LanguageCode::unknown, CountryCode::unknown };

        return { getLanguageCode (toks[0]), getCountryCode (toks[1]) };
    }

    /** @returns a printable string of the format "language code-country code";
        more specifically, this will look like "eng-ca".
    */
    String getDescription() const { return toString (code) + "-" + toString (country); }

    /** @returns true if language tag*/
    bool isValid() const noexcept
    {
        return code != LanguageCode::unknown
            && country != CountryCode::unknown;
    }

    LanguageCode code = LanguageCode::eng;
    CountryCode country = CountryCode::ca;
};

//==============================================================================
/** A representation of an IETF-compatible file for an app's
    set of translations in a particular language.

    Simply put, this maps a IETFLanguageTag to a file on disk.

    @see IETFLanguageFile, LanguageHandler
*/
struct IETFLanguageFile final
{
    /** Default constructor. */
    IETFLanguageFile() = default;

    /** Constructs a language file representation with a particular tag and file in mind. */
    IETFLanguageFile (const IETFLanguageTag& t, const File& source) :
        tag (t),
        file (source)
    {
    }

    /** @returns true if this and the other language file match. */
    bool operator== (const IETFLanguageFile& other) const noexcept { return tag == other.tag && file == other.file; }
    /** @returns true if this and the other language file do not match. */
    bool operator!= (const IETFLanguageFile& other) const noexcept { return ! operator== (other); }
    /** @returns true if this file's tag and the provided tag match. */
    bool operator== (const IETFLanguageTag& other) const noexcept { return tag == other; }
    /** @returns true if this file's tag and the provided tag do not match. */
    bool operator!= (const IETFLanguageTag& other) const noexcept { return ! operator== (other); }

    // @TODO Create a conversion system from the JUCE stuff to this stuff...
    // if (auto* const ls = LocalisedStrings::getCurrentMappings())
    //    return ls->getLanguageName();
    // return SystemStats::getUserLanguage();

    /** @returns true if the language file presently exists. */
    bool exists() const noexcept { return file.existsAsFile(); }

    /** @returns true if the tag is valid. */
    bool isValid() const noexcept { return tag.isValid(); }

    IETFLanguageTag tag;
    File file;
};

//==============================================================================
/** */
class LanguageHandler final
{
public:
    /** Constructor. */
    LanguageHandler (const File& languageDirectory);

    //==============================================================================
    /** @returns */
    const File& getLanguageDirectory() const noexcept { return languageDirectory; }

    /** */
    void addLanguageFile (const IETFLanguageFile&, bool makeActive);

    /** Changes the current language. */
    void setCurrentLanguage (const IETFLanguageTag&);

    /** Changes the current language. */
    void setCurrentLanguage (const IETFLanguageFile&);

    /** @returns */
    const IETFLanguageFile& getCurrentLanguage() const;

    /** @returns */
    const OwnedArray<IETFLanguageFile>& getAvailableLanguages() const noexcept { return languages; }

    //==============================================================================
    class Listener
    {
    public:
        virtual ~Listener()
        {
            SQUAREPINE_CRASH_TRACER
            if (parent != nullptr)
                parent->removeListener (this);
        }

        /** */
        virtual void languageChanged (const IETFLanguageFile&) = 0;

    private:
        friend class LanguageHandler;
        LanguageHandler* parent = nullptr;
    };

    /** */
    void addListener (Listener*);

    /** */
    void removeListener (Listener*);

    /** */
    void refresh();

private:
    //==============================================================================
    const File languageDirectory;
    OwnedArray<IETFLanguageFile> languages;
    int activeLanguageIndex = -1;
    ListenerList<Listener> listeners;

    //==============================================================================
    static bool isValid (const IETFLanguageFile& lf);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LanguageHandler)
};
