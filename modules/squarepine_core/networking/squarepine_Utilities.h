/** @returns a string that fixes WordPress generated URLs to that with the
    standard slash format.

    WordPress forcibly escapes the URLs (ie: \/),
    which doesn't make sense for our applications.
*/
inline String unescapeUrls (const String& url)
{
    return url.trim().replace ("\\/", "/", true);
}
