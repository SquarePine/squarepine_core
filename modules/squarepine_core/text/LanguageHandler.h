//==============================================================================
/** ISO 639-3 language code.

    @see https://en.wikipedia.org/wiki/ISO_639-1
    @see https://iso639-3.sil.org/code_tables/639/data
*/
enum class ISO6393
{
    unknown = -1,
    eng,
    fre,
    spa,
    epo,
    por,
    deu,
    nld,
    tlh //< Klingon
};

/** A more descriptive alternative to ISO 639-3. */
using LanguageCode = ISO6393;

/** @returns a language code from the provided string, or English if not matched.
    @see https://en.wikipedia.org/wiki/ISO_639-1
*/
LanguageCode getLanguageCodeFromISO6391 (const String&);

/** @returns a language code from the provided string, or English if not matched.
    @see https://en.wikipedia.org/wiki/ISO_639-2
*/
LanguageCode getLanguageCodeFromISO6392 (const String&);

/** @returns a language code from the provided string, or English if not matched.
    This will try to search via ISO 639-1 and ISO 639-2 codes.
*/
LanguageCode getLanguageCode (const String&);

/** @returns an ISO6393 formatted string representing the provided language code,
    or an empty string if it's unknown.
*/
String toString (LanguageCode);

//==============================================================================
/** ISO 3166-1 Alpha-2 country code.

    @see https://www.iban.com/country-codes
    
*/
enum class ISO31661Alpha2
{
    unknown = -1,
    us,
    ca,
    gb,
    fr,
    de,
    nl
};

/** A more descriptive alternative to ISO3166-1 Alpha-2. */
using CountryCode = ISO31661Alpha2;

/** @returns a country code from the provided string, or 'US' if not matched.
    @see https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
*/
CountryCode getCountryCodeFromISO3166Alpha2 (const String&);

/** @returns a country code from the provided string, or 'US' if not matched.
    @see https://en.wikipedia.org/wiki/ISO_3166-1_alpha-3
*/
CountryCode getCountryCodeFromISO3166Alpha3 (const String&);

/** @returns a country code from the provided string, or 'US' if not matched.
    This will try to search via ISO3166-1 Alpha-2 and ISO3166-1 Alpha-3 codes.
*/
CountryCode getCountryCode (const String&);

/** @returns an ISO3166-1 Alpha 2 formatted string representing the provided country code,
    or an empty string if it's unknown.
*/
String toString (CountryCode);

//==============================================================================
/** */
struct IETFLanguageTag final
{
    /** */
    IETFLanguageTag() noexcept = default;

    /** */
    IETFLanguageTag (LanguageCode languageCode, CountryCode countryCode) noexcept :
        code (languageCode),
        country (countryCode)
    {
    }

    /** @returns */
    bool operator== (const IETFLanguageTag& other) const noexcept { return code == other.code && country == other.country; }
    /** @returns */
    bool operator!= (const IETFLanguageTag& other) const noexcept { return ! operator== (other); }

    /** @returns */
    static IETFLanguageTag fromString (const String& s)
    {
        const auto toks = StringArray::fromTokens (s, "-", "");
        if (toks.size() != 2)
            return { LanguageCode::unknown, CountryCode::unknown };

        return { getLanguageCode (toks[0]), getCountryCode (toks[1]) };
    }

    /** @returns */
    String getDescription() const { return toString (code) + "-" + toString (country); }

    /** @returns */
    bool isValid() const noexcept { return code != LanguageCode::unknown && country != CountryCode::unknown; }

    LanguageCode code = LanguageCode::eng;
    CountryCode country = CountryCode::ca;
};

//==============================================================================
/** A representation of an IETF-compatible file for an app's
    set of translations in a particular language.
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
    LanguageHandler() = default;

    //==============================================================================
    /** */
    void initialise (const File& languageDirectory);

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
    void addListener (Listener* listener)
    {
        jassert (listener != nullptr);
        jassert (listener->parent == nullptr);

        listener->parent = this;
        listeners.add (listener);

        // To sync with this language handler.
        listener->languageChanged (getCurrentLanguage());
    }

    /** */
    void removeListener (Listener* listener) { listeners.remove (listener); }

private:
    //==============================================================================
    File languageDirectory;
    OwnedArray<IETFLanguageFile> languages;
    int activeLanguageIndex = -1;
    ListenerList<Listener> listeners;

    /** */
    static bool isValid (const IETFLanguageFile& lf);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LanguageHandler)
};

//==============================================================================
/** */
inline void updateLanguageManually (LanguageHandler& handler, LanguageHandler::Listener& listener)
{
    listener.languageChanged (handler.getCurrentLanguage());
}
