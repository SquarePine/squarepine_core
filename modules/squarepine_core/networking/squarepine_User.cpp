//==============================================================================
namespace user
{
    inline ValueTree parse (const var& s)
    {
        auto v = ValueTree (user::userId);

        if (auto* object = s.getDynamicObject())
            for (const auto& prop : object->getProperties())
                v.setProperty (prop.name, prop.value, nullptr);

        return v;
    }

    inline ValueTree parse (const OAuth2AccessPoint& accessPoint, const OAuth2Token& token)
    {
        if (token.isValid())
        {
            const auto url = accessPoint.createOAuth2MeURL (token);
            if (auto s = createEndpointStream (url, HTTPRequest::GET))
            {
                //NB: Although this isn't technically a URL, the result
                //    may contain URLs inside the (potential) JSON.
                const auto result = unescapeUrls (s->readEntireStreamAsString());
                return parse (JSON::parse (result));
            }
        }

        return {};
    }
}

//==============================================================================
User::User() :
    state (user::userId)
{
}

User::User (const ValueTree& s) :
    state (s)
{
    if (! state.hasType (user::userId))
    {
        jassertfalse; //Not sure what happened here...
        state = ValueTree (user::userId); //Dirty invalidation and recovery...
    }
}

User::User (const User& other) :
    token (other.token),
    state (other.state)
{
}

User::~User()
{
    /** @warning It is absolutely ESSENTIAL that the user instance is logged out.
        If you don't do that, you'll leave the web servers in a crazy state and the
        user will never be able to recover.
    */
    jassert (token.isNull());
}

//==============================================================================
User::Ptr User::login (OAuth2AccessPoint& accessPoint, const String& username, const String& password)
{
    if (username.isNotEmpty() && password.isNotEmpty())
    {
        auto token = accessPoint.login (username, password);
        if (token.isValid())
        {
            auto user = make_refptr<User>();
            user->token = std::move (token);
            user->state = user::parse (accessPoint, user->token);
            return user;
        }
    }

    return {};
}

bool User::refreshUserToken (OAuth2AccessPoint& accessPoint, User::Ptr user)
{
    if (user != nullptr && user->token.isValid())
    {
        user->token = accessPoint.refresh ({}, user->token);
        return user->token.isValid();
    }

    return false;
}

bool User::logout (OAuth2AccessPoint& accessPoint, User::Ptr user)
{
    if (user != nullptr && user->token.isValid())
    {
        if (accessPoint.logout (user->token))
            user->token = {};

        return user->token.isNull();
    }

    return false;
}

//==============================================================================
User& User::operator= (const User& other)
{
    token = other.token;
    state = other.state;
    return *this;
}

bool User::operator== (const User& other) const noexcept        { return token == other.token; }
bool User::operator== (const User::Ptr other) const noexcept    { return other != nullptr && operator== (*other.get()); }
bool User::operator!= (const User& other) const noexcept        { return ! operator== (other); }
bool User::operator!= (const User::Ptr other) const noexcept    { return ! operator== (other); }
