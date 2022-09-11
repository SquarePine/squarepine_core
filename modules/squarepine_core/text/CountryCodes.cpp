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
