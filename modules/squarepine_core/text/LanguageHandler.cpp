namespace
{
    const auto languageFileExtension = String (".language");
}

//==============================================================================
LanguageCode getLanguageCodeFromISO6391 (const String& s)
{
    if (s.equalsIgnoreCase ("en")) return LanguageCode::eng;
    if (s.equalsIgnoreCase ("fr")) return LanguageCode::fre;
    if (s.equalsIgnoreCase ("es")) return LanguageCode::spa;
    if (s.equalsIgnoreCase ("eo")) return LanguageCode::epo;
    if (s.equalsIgnoreCase ("pt")) return LanguageCode::por;
    if (s.equalsIgnoreCase ("de")) return LanguageCode::deu;
    if (s.equalsIgnoreCase ("nl")) return LanguageCode::nld;

    // "klingon", tlh // ISO 639-1 is not available.

    return LanguageCode::unknown;
}

LanguageCode getLanguageCodeFromISO6392 (const String& s)
{
    if (s.equalsIgnoreCase ("eng")) return LanguageCode::eng;
    if (s.equalsIgnoreCase ("fre")) return LanguageCode::fre;
    if (s.equalsIgnoreCase ("spa")) return LanguageCode::spa;
    if (s.equalsIgnoreCase ("epo")) return LanguageCode::epo;
    if (s.equalsIgnoreCase ("por")) return LanguageCode::por;
    if (s.equalsIgnoreCase ("deu")) return LanguageCode::deu;
    if (s.equalsIgnoreCase ("nld")) return LanguageCode::nld;
    if (s.equalsIgnoreCase ("tlh")) return LanguageCode::tlh;

    return LanguageCode::unknown;
}

LanguageCode getLanguageCode (const String& s)
{
    const auto lc = getLanguageCodeFromISO6392 (s);
    if (lc != LanguageCode::unknown)
        return lc;

    return getLanguageCodeFromISO6391 (s);
}

String toString (LanguageCode lc)
{
    switch (lc)
    {
        case LanguageCode::eng: return "eng";
        case LanguageCode::fre: return "fre";
        case LanguageCode::spa: return "spa";
        case LanguageCode::epo: return "epo";
        case LanguageCode::por: return "por";
        case LanguageCode::deu: return "deu";
        case LanguageCode::nld: return "nld";
        case LanguageCode::tlh: return "tlh";

        default: break;
    };

    return {};
}

//==============================================================================
CountryCode getCountryCodeFromISO3166Alpha2 (const String& s)
{
    if (s.equalsIgnoreCase ("us")) return CountryCode::us;
    if (s.equalsIgnoreCase ("ca")) return CountryCode::ca;
    if (s.equalsIgnoreCase ("gb")) return CountryCode::gb;
    if (s.equalsIgnoreCase ("fr")) return CountryCode::fr;
    if (s.equalsIgnoreCase ("de")) return CountryCode::de;
    if (s.equalsIgnoreCase ("nl")) return CountryCode::nl;

    return CountryCode::unknown;
}

CountryCode getCountryCodeFromISO3166Alpha3 (const String& s)
{
    if (s.equalsIgnoreCase ("usa")) return CountryCode::us;
    if (s.equalsIgnoreCase ("can")) return CountryCode::ca;
    if (s.equalsIgnoreCase ("gbr")) return CountryCode::gb;
    if (s.equalsIgnoreCase ("fra")) return CountryCode::fr;
    if (s.equalsIgnoreCase ("deu")) return CountryCode::de;
    if (s.equalsIgnoreCase ("nld")) return CountryCode::nl;

    return CountryCode::unknown;
}

CountryCode getCountryCode (const String& s)
{
    const auto lc = getCountryCodeFromISO3166Alpha3 (s);
    if (lc != CountryCode::unknown)
        return lc;

    return getCountryCodeFromISO3166Alpha2 (s);
}

String toString (CountryCode cc)
{
    switch (cc)
    {
        case CountryCode::us: return "us";
        case CountryCode::ca: return "ca";
        case CountryCode::gb: return "gb";
        case CountryCode::fr: return "fr";
        case CountryCode::de: return "de";
        case CountryCode::nl: return "nl";

        default: break;
    };

    return {};
}

//==============================================================================
void LanguageHandler::initialise (const File& langDir)
{
    if (languageDirectory == langDir)
        return;

    languageDirectory = langDir;

    if (! languageDirectory.isDirectory())
    {
        const auto r = languageDirectory.createDirectory();

        if (! r.wasOk())
        {
            Logger::writeToLog ("Error! Failed to create language translations directory!\n");

            if (r.getErrorMessage().isNotEmpty())
                Logger::writeToLog ("Message: " + r.getErrorMessage());

            jassertfalse;
            languageDirectory = File();
            return;
        }
    }

    Array<File> languageFiles;
    languageFiles.minimiseStorageOverheads();
    languageDirectory.findChildFiles (languageFiles, File::findFiles, false, "*" + languageFileExtension);

    IETFLanguageFile ietfDefaultLang;
    const auto defaultLang = languageDirectory.getChildFile (ietfDefaultLang.tag.getDescription() + languageFileExtension);

    if (languageFiles.isEmpty()
        || ! defaultLang.existsAsFile()
        || defaultLang.getSize() < 64
        || defaultLang.isSymbolicLink())
    {
        defaultLang.deleteRecursively (false);

        auto fs = FileOutputStream (defaultLang);
        if (fs.failedToOpen())
        {
            Logger::writeToLog ("Error! Unable to create default language file for some reason...\nSee path:\n\t"  + defaultLang.getFullPathName());
            jassertfalse;
            return;
        }

        if (auto* ls = LocalisedStrings::getCurrentMappings())
        {
            const auto& mappings = ls->getMappings();

            for (const auto& key : mappings.getAllKeys())
                fs << key << " = " << mappings.getValue (key, {});
        }

        ietfDefaultLang.file = defaultLang;
        addLanguageFile (ietfDefaultLang, true);
    } 

    for (const auto& lf : languageFiles)
    {
        IETFLanguageFile f;
        f.file = lf;
        f.tag = IETFLanguageTag::fromString (f.file.getFileNameWithoutExtension());

        if (f.tag.isValid())
        {
            addLanguageFile (f, false);
        }
        else
        {
            Logger::writeToLog ("Error! Unknown or broken language file...\nSee path:\n\t" + lf.getFullPathName());
            jassertfalse;
        }
    }
}

bool LanguageHandler::isValid (const IETFLanguageFile& lf)
{
    return lf.exists() && lf.isValid() && lf.file.getFileExtension() == languageFileExtension;
}

void LanguageHandler::addLanguageFile (const IETFLanguageFile& lf, bool makeActive)
{
    if (! isValid (lf))
    {
        Logger::writeToLog ("Unknown or language file...\nSee path:\n\t"  + lf.file.getFullPathName());
        jassertfalse;
        return;
    }

    if (languages.isEmpty())
        makeActive = true;

    // @TODO: for now, assume the language file is unique...
    languages.add (new IETFLanguageFile (lf));

    if (makeActive)
        setCurrentLanguage (lf);
}

void LanguageHandler::setCurrentLanguage (const IETFLanguageTag& lt)
{
    for (int i = languages.size(); --i >= 0;)
    {
        if (auto* lang = languages.getUnchecked (i))
        {
            if (*lang == lt)
            {
                if (activeLanguageIndex == i)
                    return; //Nothing to do...

                activeLanguageIndex = i;

                listeners.call ([&] (Listener& l) { l.languageChanged (*lang); });
                return;
            }
        }
    }

    Logger::writeToLog (String ("Error! Tried to switch to an unknown language: ")
                        + String ((int) lt.code) + "-" + String ((int) lt.country));
    jassertfalse;
}

void LanguageHandler::setCurrentLanguage (const IETFLanguageFile& lf)
{
    if (! isValid (lf))
    {
        Logger::writeToLog ("Invalid language file!\nSee path:\n\t"  + lf.file.getFullPathName());
        jassertfalse;
        return;
    }

    for (int i = languages.size(); --i >= 0;)
    {
        if (auto* lang = languages.getUnchecked (i))
        {
            if (*lang == lf)
            {
                setCurrentLanguage (lf.tag);
                return;
            }
        }
    }
}

const IETFLanguageFile& LanguageHandler::getCurrentLanguage() const
{
    jassert (! languages.isEmpty() && activeLanguageIndex >= 0);

    return *languages.getFirst();
}
