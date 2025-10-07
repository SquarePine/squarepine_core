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
