//==============================================================================
/** */
struct OAuth2Token final
{
    /** */
    OAuth2Token() = default;

    /** */
    OAuth2Token (const String& accessToken, const String& tokenType,
                 const String& refreshToken, const String& scope,
                 int64 expiryTime);

    /** */
    OAuth2Token (const OAuth2Token& other);

    /** */
    OAuth2Token (OAuth2Token&& other) noexcept;

    /** */
    ~OAuth2Token() = default;

    /** */
    bool isValid() const;

    /** */
    bool isNull() const;

    OAuth2Token& operator= (const OAuth2Token& other);
    OAuth2Token& operator= (OAuth2Token&& other) noexcept;
    bool operator== (const OAuth2Token& other) const;
    bool operator!= (const OAuth2Token& other) const;

    String accessToken, tokenType, refreshToken, scope;
    int64 expiresIn = -1;
};

//==============================================================================
/** */
struct OAuth2AccessSymbol final
{
    /** */
    OAuth2AccessSymbol() = default;

    /** */
    OAuth2AccessSymbol (const String& key, const String& value);

    /** */
    OAuth2AccessSymbol (const OAuth2AccessSymbol& other);

    /** */
    ~OAuth2AccessSymbol() = default;

    URL applyToUrl (const URL& url) const;

    OAuth2AccessSymbol& operator= (const OAuth2AccessSymbol& other);
    bool operator== (const OAuth2AccessSymbol& other) const;
    bool operator!= (const OAuth2AccessSymbol& other) const;

    String key, value;
};

//==============================================================================
/** */
class OAuth2AccessPoint
{
public:
    /** */
    OAuth2AccessPoint (const OAuth2AccessSymbol& key, const OAuth2AccessSymbol& secret) noexcept;

    /** */
    virtual ~OAuth2AccessPoint() noexcept;

    /** */
    virtual String getDomain() const = 0;

    /** */
    virtual String getOAuth2EndpointPath() const = 0;

    /** */
    virtual String getOAuth2EndpointRevokePath() const = 0;

    /** */
    virtual String getOAuth2EndpointMePath() const = 0;

    /** */
    URL createOAuth2URL() const;

    /** */
    URL createOAuth2MeURL (const OAuth2Token& token) const;

    /** */
    const OAuth2AccessSymbol& getKey() const noexcept { return key; }

    /** */
    const OAuth2AccessSymbol& getSecret() const noexcept { return secret; }

    /** */
    void setBasicAuthorisationHeader (StringPairArray& destination) const;

    /** */
    void setBearerAuthorisationHeader (StringPairArray& destination, const OAuth2Token& token) const;

    /** */
    URL createLoginWithPasswordURL (const String& username, const String& password) const;

    /** */
    URL createLoginWithClientCredentialsURL() const;

    /** */
    OAuth2Token login (const URL& url, const StringPairArray& headers = {}) const;

    /** */
    virtual OAuth2Token login (const String& username, const String& password) const;

    /** */
    virtual bool logout (const OAuth2Token& token) const;

    /** */
    OAuth2Token refresh (const URL& url, const OAuth2Token& token) const;

    /** */
    var fetchMe (const OAuth2Token& token) const;

    /** */
    virtual URL createDomainAccessURLWithPath (const String& path) const;

protected:
    const OAuth2AccessSymbol key, secret;

private:
    OAuth2AccessPoint() = delete;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OAuth2AccessPoint)
};
