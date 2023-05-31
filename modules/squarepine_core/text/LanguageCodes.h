#pragma once

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
    tlh             // Klingon
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
