#pragma once

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
