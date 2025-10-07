//==============================================================================
OAuth2Token::OAuth2Token (const String& at, const String& tt, const String& rt, const String& scp, int64 expiryTime) :
    accessToken (at.trim()),
    tokenType (tt.trim()),
    refreshToken (rt.trim()),
    scope (scp.trim()),
    expiresIn (expiryTime)
{
}

OAuth2Token::OAuth2Token (const OAuth2Token& other) :
    accessToken (other.accessToken),
    tokenType (other.tokenType),
    refreshToken (other.refreshToken),
    scope (other.scope),
    expiresIn (other.expiresIn)
{
}

OAuth2Token::OAuth2Token (OAuth2Token&& other) noexcept :
    accessToken (other.accessToken),
    tokenType (other.tokenType),
    refreshToken (other.refreshToken),
    scope (other.scope),
    expiresIn (other.expiresIn)
{
}

OAuth2Token& OAuth2Token::operator= (const OAuth2Token& other)
{
    accessToken = other.accessToken;
    tokenType = other.tokenType;
    refreshToken = other.refreshToken;
    scope = other.scope;
    expiresIn = other.expiresIn;
    return *this;
}

OAuth2Token& OAuth2Token::operator= (OAuth2Token&& other) noexcept
{
    accessToken = std::move (other.accessToken);
    tokenType = std::move (other.tokenType);
    refreshToken = std::move (other.refreshToken);
    scope = std::move (other.scope);
    expiresIn = other.expiresIn;
    return *this;
}

bool OAuth2Token::operator== (const OAuth2Token& other) const
{
    return accessToken == other.accessToken;
}

bool OAuth2Token::operator!= (const OAuth2Token& other) const
{
    return ! operator== (other);
}

bool OAuth2Token::isValid() const
{
    return accessToken.isNotEmpty() && tokenType.isNotEmpty();
}

bool OAuth2Token::isNull() const
{
    return ! isValid();
}

//==============================================================================
OAuth2AccessSymbol::OAuth2AccessSymbol (const String& k, const String& v) :
    key (k.trim()),
    value (v.trim())
{
    jassert (key.isNotEmpty());
    jassert (value.isNotEmpty());
}

OAuth2AccessSymbol::OAuth2AccessSymbol (const OAuth2AccessSymbol& other) :
    key (other.key),
    value (other.value)
{
    jassert (key.isNotEmpty());
    jassert (value.isNotEmpty());
}

URL OAuth2AccessSymbol::applyToUrl (const URL& url) const
{
    return url.withParameter (key, value);
}

OAuth2AccessSymbol& OAuth2AccessSymbol::operator= (const OAuth2AccessSymbol& other)
{
    key = other.key;
    value = other.value;
    return *this;
}

bool OAuth2AccessSymbol::operator== (const OAuth2AccessSymbol& other) const
{
    return key == other.key && value == other.value;
}

bool OAuth2AccessSymbol::operator!= (const OAuth2AccessSymbol& other) const
{
    return ! operator== (other);
}

//==============================================================================
OAuth2AccessPoint::OAuth2AccessPoint (const OAuth2AccessSymbol& k, const OAuth2AccessSymbol& s) noexcept :
    key (k),
    secret (s)
{
}

OAuth2AccessPoint::~OAuth2AccessPoint() noexcept
{
}

URL OAuth2AccessPoint::createOAuth2URL() const
{
    auto url = URL (createFilteredUrl (getDomain(), getOAuth2EndpointPath()));
    url = key.applyToUrl (url);
    return secret.applyToUrl (url);
}

URL OAuth2AccessPoint::createOAuth2MeURL (const OAuth2Token& token) const
{
    if (token.isNull())
    {
        jassertfalse;
        return {};
    }

    return URL (createFilteredUrl (getDomain(), getOAuth2EndpointMePath()))
            .withParameter ("access_token", token.accessToken);
}

void OAuth2AccessPoint::setBasicAuthorisationHeader (StringPairArray& destination) const
{
    destination.set ("Authorization:", "Basic " + Base64::toBase64 (key.key + ":" + secret.key));
}

void OAuth2AccessPoint::setBearerAuthorisationHeader (StringPairArray& destination, const OAuth2Token& token) const
{
    jassert (token.isValid());
    if (token.isValid())
        destination.set ("Authorization:", "Bearer " + token.accessToken);
}

URL OAuth2AccessPoint::createLoginWithPasswordURL (const String& username, const String& password) const
{
    auto url = createOAuth2URL();

    StringPairArray params;
    params.set ("grant_type", "password");
    params.set ("response_type", "code"); //TODO this might not be needed for all OAuth2 systems...
    params.set ("username", username);
    params.set ("password", password);

    return url.withParameters (params);
}

URL OAuth2AccessPoint::createLoginWithClientCredentialsURL() const
{
    return createOAuth2URL().withParameter ("grant_type", "client_credentials");
}

OAuth2Token OAuth2AccessPoint::login (const URL& url, const StringPairArray& headers) const
{
    auto parseStringProperty = [] (DynamicObject& dynObj, const String& name)
    {
        return dynObj.getProperty (name).toString().trim();
    };

    if (auto stream = createEndpointStream (url, HTTPRequest::POST, headers))
    {
        const auto body = stream->readEntireStreamAsString().trim();

       #if SQUAREPINE_LOG_NETWORK_CALLS
        Logger::writeToLog (String ("Received result:") + newLine + body);
       #endif

        const auto result = JSON::parse (body);
        if (auto* object = result.getDynamicObject())
        {
            return
            {
                parseStringProperty (*object, "access_token"),
                parseStringProperty (*object, "token_type"),
                parseStringProperty (*object, "refresh_token"),
                parseStringProperty (*object, "scope"),
                static_cast<int64> (object->getProperty ("expires_in"))
            };
        }

       #if SQUAREPINE_LOG_NETWORK_CALLS
        if (! result.isVoid() && ! result.isUndefined())
            Logger::writeToLog ("Failed to login!");
       #endif
    }
    else
    {
       #if SQUAREPINE_LOG_NETWORK_CALLS
        Logger::writeToLog ("OAuth2 Failure: couldn't login.");
       #endif
    }

    return {};
}

OAuth2Token OAuth2AccessPoint::login (const String& username, const String& password) const
{
    StringPairArray spa;
    setBasicAuthorisationHeader (spa);
    setContentTypeAppFormEncoded (spa);

    return login (createLoginWithPasswordURL (username, password), spa);
}

bool OAuth2AccessPoint::logout (const OAuth2Token& token) const
{
    if (token.isNull())
        return true; //Nothing to do I guess?

    auto url = URL (createFilteredUrl (getDomain(), getOAuth2EndpointRevokePath()));
    url = url.withParameter ("access_token", token.accessToken);

    if (auto stream = createEndpointStream (url, HTTPRequest::GET))
    {
        const auto body = stream->readEntireStreamAsString();

       #if SQUAREPINE_LOG_NETWORK_CALLS
        Logger::writeToLog (String ("Received result:") + newLine + body);
       #endif

        if (stream->getStatusCode() == 200)
        {
            const auto result = JSON::parse (body);
            if (auto* object = result.getDynamicObject())
                return static_cast<bool> (object->getProperty ("status"));
        }
    }

    return false;
}

OAuth2Token OAuth2AccessPoint::refresh (const URL&, const OAuth2Token&) const
{
    jassertfalse; //TODO
    return {};
}

var OAuth2AccessPoint::fetchMe (const OAuth2Token& token) const
{
    auto url = createOAuth2MeURL (token);

    if (auto stream = createEndpointStream (url, HTTPRequest::GET))
    {
        const auto body = stream->readEntireStreamAsString();

       #if SQUAREPINE_LOG_NETWORK_CALLS
        Logger::writeToLog (String ("Received result:") + newLine + body);
       #endif

        return JSON::parse (body);
    }

    return {};
}

URL OAuth2AccessPoint::createDomainAccessURLWithPath (const String& path) const
{
    auto url = URL (createFilteredUrl (getDomain(), path));
    url = key.applyToUrl (url);
    return secret.applyToUrl (url);
}
