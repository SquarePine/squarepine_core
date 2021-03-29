//==============================================================================
WordPressAccessPoint::WordPressAccessPoint (const String& clientId, const String& clientSecret) :
    OAuth2AccessPoint ({ "client_id", clientId },
                       { "client_secret", clientSecret })
{
}

String WordPressAccessPoint::getOAuth2EndpointPath() const
{
    //@see https://wp-oauth.com/docs/general/grant-types/refresh-token/
    return "/oauth/token";
}

String WordPressAccessPoint::getOAuth2EndpointRevokePath() const
{
    //@see https://wp-oauth.com/docs/general/endpoints/destroy/
    return "/oauth/destroy";
}

String WordPressAccessPoint::getOAuth2EndpointMePath() const
{
    return "/oauth/me";
}

//==============================================================================
WooCommerceAccessPoint::WooCommerceAccessPoint (const String& clientId, const String& clientSecret) :
    OAuth2AccessPoint ({ "consumer_key", clientId },
                       { "consumer_secret", clientSecret })
{
}

String WooCommerceAccessPoint::getOAuth2EndpointPath() const
{
    return "/wp-json/wc/v3";
}

String WooCommerceAccessPoint::getOAuth2EndpointRevokePath() const
{
    return "/oauth/destroy";
}

String WooCommerceAccessPoint::getOAuth2EndpointMePath() const
{
    return "/oauth/me";
}

URL WooCommerceAccessPoint::createDomainAccessURLWithPath (const String& path) const
{
    auto url = URL (createFilteredUrl (createFilteredUrl (getDomain(), "/wp-json/wc/v3"), path));
    url = key.applyToUrl (url);
    return secret.applyToUrl (url);
}
