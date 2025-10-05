#if SQUAREPINE_USE_GOOGLE_API

//==============================================================================
// Static scope definitions
const String GoogleAPIManager::Scopes::SHEETS_READWRITE = "https://www.googleapis.com/auth/spreadsheets";
const String GoogleAPIManager::Scopes::DOCS_READWRITE = "https://www.googleapis.com/auth/documents";
const String GoogleAPIManager::Scopes::GMAIL_READWRITE = "https://www.googleapis.com/auth/gmail.modify";
const String GoogleAPIManager::Scopes::CALENDAR_READWRITE = "https://www.googleapis.com/auth/calendar";
const String GoogleAPIManager::Scopes::USERINFO_EMAIL = "https://www.googleapis.com/auth/userinfo.email";
const String GoogleAPIManager::Scopes::USERINFO_PROFILE = "https://www.googleapis.com/auth/userinfo.profile";

//==============================================================================
// Implementation

void GoogleAPIManager::setClientCredentials (const String& clientId, const String& clientSecret)
{
    config_.clientId = clientId;
    config_.clientSecret = clientSecret;
}

void GoogleAPIManager::setConfig (const Config& config)
{
    config_ = config;
}

void GoogleAPIManager::addScope (const String& scope)
{
    if (!config_.scopes.contains (scope))
        config_.scopes.add (scope);
}

void GoogleAPIManager::setRedirectUri (const String& uri)
{
    config_.redirectUri = uri;
}

String GoogleAPIManager::getAuthorizationUrl()
{
    if (config_.clientId.isEmpty())
    {
        jassertfalse; // Client ID not set
        return {};
    }

    URL url ("https://accounts.google.com/o/oauth2/v2/auth");
    return url.withParameter ("client_id", config_.clientId)
             .withParameter ("redirect_uri", config_.redirectUri)
             .withParameter ("scope", buildScopesString())
             .withParameter ("response_type", "code")
             .withParameter ("access_type", "offline")
             .withParameter ("prompt", "consent")
             .toString (false);
}

void GoogleAPIManager::authenticate (AuthCallback callback)
{
    // Try to load existing credentials first
    if (loadCredentials() && ! needsTokenRefresh())
    {
        callback (Result::ok());
        return;
    }

    // If we have a refresh token, use it
    if (! credentials_.refreshToken.isEmpty())
    {
        refreshAccessToken (callback);
        return;
    }

    // Otherwise, user needs to authorise manually
    callback (false, "Manual authorisation required. Use getAuthorizationUrl() and authenticateWithCode()");
}

void GoogleAPIManager::authenticateWithCode (const String& authCode, AuthCallback callback)
{
    exchangeCodeForToken (authCode, callback);
}

bool GoogleAPIManager::isAuthenticated() const
{
    ScopedLock lock (credentialsLock_);
    return ! credentials_.accessToken.isEmpty() && Time::getCurrentTime() < credentials_.expiresAt;
}

void GoogleAPIManager::clearCredentials()
{
    ScopedLock lock (credentialsLock_);
    credentials_ = Credentials{};

    // Delete credentials file
    const auto credFile = File::getCurrentWorkingDirectory().getChildFile (config_.credentialsFile);
    if (credFile.exists())
        credFile.deleteFile();
}

void GoogleAPIManager::getSheetsData (const String& spreadsheetId, const String& range, DataCallback callback)
{
    const auto url = getSheetsUrl ("spreadsheets/" + spreadsheetId + "/values/" + urlEncode (range));
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::updateSheetsData (const String& spreadsheetId, const String& range,
                                        const var& values, BoolCallback callback)
{
    const     auto url = getSheetsUrl ("spreadsheets/" + spreadsheetId + "/values/" + urlEncode (range))
               .withParameter ("valueInputOption", "RAW");

    auto requestBody = new DynamicObject();
    requestBody->setProperty ("values", values);

    String postData = JSON::toString (var (requestBody));

    makeAuthenticatedRequest (url, "PUT", postData, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

void GoogleAPIManager::appendSheetsData (const String& spreadsheetId, const String& range,
                                        const var& values, BoolCallback callback)
{
    const auto url = getSheetsUrl ("spreadsheets/" + spreadsheetId + "/values/" + urlEncode (range) + ":append")
               .withParameter ("valueInputOption", "RAW");

    auto requestBody = new DynamicObject();
    requestBody->setProperty ("values", values);

    String postData = JSON::toString (var (requestBody));

    makeAuthenticatedRequest (url, "POST", postData, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

void GoogleAPIManager::createSpreadsheet (const String& title, DataCallback callback)
{
    const auto url = getSheetsUrl ("spreadsheets");

    auto properties = new DynamicObject();
    properties->setProperty ("title", title);
    
    auto requestBody = new DynamicObject();
    requestBody->setProperty ("properties", var (properties));

    const auto postData = JSON::toString (var (requestBody));
    makeAuthenticatedRequest (url, "POST", postData, callback);
}

void GoogleAPIManager::getSpreadsheetInfo (const String& spreadsheetId, DataCallback callback)
{
    const auto url = getSheetsUrl ("spreadsheets/" + spreadsheetId);
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::getDocument (const String& documentId, DataCallback callback)
{
    const auto url = getDocsUrl ("documents/" + documentId);
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::createDocument (const String& title, DataCallback callback)
{
    const auto url = getDocsUrl ("documents");

    auto requestBody = new DynamicObject();
    requestBody->setProperty ("title", title);

    const auto postData = JSON::toString (var (requestBody));
    makeAuthenticatedRequest (url, "POST", postData, callback);
}

void GoogleAPIManager::updateDocument (const String& documentId, const var& requests, BoolCallback callback)
{
    const auto url = getDocsUrl ("documents/" + documentId + ":batchUpdate");

    auto requestBody = new DynamicObject();
    requestBody->setProperty ("requests", requests);

    const auto postData = JSON::toString (var (requestBody));

    makeAuthenticatedRequest (url, "POST", postData, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

void GoogleAPIManager::insertText (const String& documentId, int index, const String& text, BoolCallback callback)
{
    auto location = new DynamicObject();
    location->setProperty ("index", index);
    
    auto insertTextRequest = new DynamicObject();
    insertTextRequest->setProperty ("location", var (location));
    insertTextRequest->setProperty ("text", text);

    auto request = new DynamicObject();
    request->setProperty ("insertText", var (insertTextRequest));

    var requests;
    requests.append (var (request));

    updateDocument (documentId, requests, callback);
}

void GoogleAPIManager::getMessages (const String& query, int maxResults, DataCallback callback)
{
    auto url = getGmailUrl ("users/me/messages")
                    .withParameter ("maxResults", String (maxResults));

    if (query.isNotEmpty())
        url = url.withParameter ("q", query);

    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::getMessage (const String& messageId, DataCallback callback)
{
    const auto url = getGmailUrl ("users/me/messages/" + messageId);
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::sendEmail (const String& to, const String& subject, const String& body,
                                 const String& from, BoolCallback callback)
{
    String fromAddr = from.isNotEmpty() ? from : "me";

    // Build email message
    String message = "To: " + to + "\r\n";
    message += "Subject: " + subject + "\r\n";
    message += "Content-Type: text/plain; charset=utf-8\r\n";
    message += "\r\n";
    message += body;

    auto requestBody = new DynamicObject();
    requestBody->setProperty ("raw", Base64::toBase64 (message));

    const auto url = getGmailUrl ("users/me/messages/send");
    const auto postData = JSON::toString (var (requestBody));

    makeAuthenticatedRequest (url, "POST", postData, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

void GoogleAPIManager::getProfile (DataCallback callback)
{
    const auto url = getGmailUrl ("users/me/profile");
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::getLabels (DataCallback callback)
{
    const auto url = getGmailUrl ("users/me/labels");
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::getCalendars (DataCallback callback)
{
    const auto url = getCalendarUrl ("users/me/calendarList");
    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::getEvents (const String& calendarId, const String& timeMin,
                                 const String& timeMax, DataCallback callback)
{
    auto url = getCalendarUrl ("calendars/" + urlEncode (calendarId) + "/events");

    if (timeMin.isNotEmpty())
        url = url.withParameter ("timeMin", timeMin);
    if (timeMax.isNotEmpty())
        url = url.withParameter ("timeMax", timeMax);

    makeAuthenticatedRequest (url, "GET", {}, callback);
}

void GoogleAPIManager::createEvent (const String& calendarId, const var& event, DataCallback callback)
{
    const auto url = getCalendarUrl ("calendars/" + urlEncode (calendarId) + "/events");
    const auto postData = JSON::toString (event);
    makeAuthenticatedRequest (url, "POST", postData, callback);
}

void GoogleAPIManager::updateEvent (const String& calendarId, const String& eventId,
                                   const var& event, BoolCallback callback)
{
    const auto url = getCalendarUrl ("calendars/" + urlEncode (calendarId) + "/events/" + eventId);
    const auto postData = JSON::toString (event);

    makeAuthenticatedRequest (url, "PUT", postData, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

void GoogleAPIManager::deleteEvent (const String& calendarId, const String& eventId, BoolCallback callback)
{
    const auto url = getCalendarUrl ("calendars/" + urlEncode (calendarId) + "/events/" + eventId);

    makeAuthenticatedRequest (url, "DELETE", {}, [callback] (const var& data, const String& error) {
        callback (error.isEmpty(), error);
    });
}

String GoogleAPIManager::getAccessToken() const
{
    ScopedLock lock (credentialsLock_);
    return credentials_.accessToken;
}

bool GoogleAPIManager::needsTokenRefresh() const
{
    ScopedLock lock (credentialsLock_);
    return credentials_.accessToken.isEmpty()
           || (Time::getCurrentTime() >= (credentials_.expiresAt - RelativeTime::seconds (60)));
}

void GoogleAPIManager::refreshToken (AuthCallback callback)
{
    refreshAccessToken (callback);
}

void GoogleAPIManager::saveCredentials()
{
    ScopedLock lock (credentialsLock_);

    auto credentialsJson = new DynamicObject();
    credentialsJson->setProperty ("access_token", credentials_.accessToken);
    credentialsJson->setProperty ("refresh_token", credentials_.refreshToken);
    credentialsJson->setProperty ("token_type", credentials_.tokenType);
    credentialsJson->setProperty ("expires_at", (int64) credentials_.expiresAt.toMilliseconds());

    var scopesArray;
    for (const auto& scope : credentials_.scopes)
        scopesArray.append (scope);

    credentialsJson->setProperty ("scopes", scopesArray);

    File credFile (File::getCurrentWorkingDirectory().getChildFile (config_.credentialsFile));
    credFile.replaceWithText (JSON::toString (var (credentialsJson)));
}

bool GoogleAPIManager::loadCredentials()
{
    const auto credFile = File::getCurrentWorkingDirectory().getChildFile (config_.credentialsFile);
    if (! credFile.exists())
        return false;

    const auto jsonText = credFile.loadFileAsString();
    if (jsonText.isEmpty())
        return false;

    auto credentialsJson = JSON::parse (jsonText);
    if (! credentialsJson.isObject())
        return false;

    ScopedLock lock (credentialsLock_);

    credentials_.accessToken = credentialsJson["access_token"].toString();
    credentials_.refreshToken = credentialsJson["refresh_token"].toString();
    credentials_.tokenType = credentialsJson["token_type"].toString();
    credentials_.expiresAt = Time ((int64) credentialsJson["expires_at"]);

    credentials_.scopes.clear();
    if (credentialsJson["scopes"].isArray())
    {
        auto scopesArray = credentialsJson["scopes"].getArray();
        for (const auto& scope : *scopesArray)
            credentials_.scopes.add (scope.toString());
    }

    return ! credentials_.accessToken.isEmpty();
}

void GoogleAPIManager::makeRequest (URL url, const String& method, const String& postData,
                                    const String& headers, DataCallback callback)
{
    jassert (callback != nullptr);

    const auto options = URL::InputStreamOptions (URL::ParameterHandling::inAddress)
                           .withConnectionTimeoutMs (config_.timeoutMs)
                           .withResponseHeaders (nullptr)
                           .withExtraHeaders (headers)
                           .withHttpRequestCmd (method);

    if (postData.isNotEmpty())
        url = url.withPOSTData (postData);

    auto stream = url.createInputStream (options);
    if (stream == nullptr)
    {
        callback ({}, "Request failed or was cancelled");
        return;
    }

    const auto response = stream->readEntireStreamAsString();

    auto result = Result::ok();
    const auto data = JSON::parse (response);

    if (! data.isObject())
    {
        result = Result::fail ("Invalid JSON response");
    }
    else if (data.hasProperty ("error"))
    {
        const auto& errorObj = data["error"];
        if (errorObj.isObject())
            result = Result::fail (errorObj["message"].toString());
        else
            result = Result::fail (errorObj.toString());
    }

    callback (data, result);
}

void GoogleAPIManager::makeRequest (URL url, const String& method, const String& postData,
                                    const StringPairArray& headers, DataCallback callback)
{
    makeRequest (url, method, postData, toString (headers), callback);
}

void GoogleAPIManager::makeAuthenticatedRequest (URL url, const String& method,
                                                 const String& postData, DataCallback callback)
{
    if (! isAuthenticated())
    {
        if (! credentials_.refreshToken.isEmpty())
        {
            refreshAccessToken ([this, url, method, postData, callback] (bool success, const String& error) {
                if (success)
                    makeAuthenticatedRequest (url, method, postData, callback);
                else
                    callback (var(), "Authentication failed: " + error);
            });
            return;
        }

        callback (var(), "Not authenticated. Please call authenticate() first.");
        return;
    }

    auto headers = getAuthHeaders();
    if (method == "POST" || method == "PUT" || method == "PATCH")
        headers.set ("Content-Type", "application/json");

    makeRequest (url, method, postData, headers, callback);
}

void GoogleAPIManager::exchangeCodeForToken (const String& code, AuthCallback callback)
{
    URL url ("https://oauth2.googleapis.com/token");

    StringPairArray postParams;
    postParams.set ("code", code);
    postParams.set ("client_id", config_.clientId);
    postParams.set ("client_secret", config_.clientSecret);
    postParams.set ("redirect_uri", config_.redirectUri);
    postParams.set ("grant_type", "authorization_code");

    String postData;
    for (int i = 0; i < postParams.size(); ++i)
    {
        if (i > 0)
            postData << "&";

        postData += urlEncode (postParams.getAllKeys()[i]) + "=" + urlEncode (postParams.getAllValues()[i]);
    }

    StringPairArray headers;
    headers.set ("Content-Type", "application/x-www-form-urlencoded");

    makeRequest (url, "POST", postData, headers, [this, callback] (const var& data, const String& error) {
        if (! error.isEmpty())
        {
            callback (false, error);
            return;
        }

        ScopedLock lock (credentialsLock_);
        credentials_.accessToken    = data["access_token"].toString();
        credentials_.refreshToken   = data["refresh_token"].toString();
        credentials_.tokenType      = data["token_type"].toString();

        const auto expiresIn = static_cast<int> (data["expires_in"]);
        credentials_.expiresAt = Time::getCurrentTime() + RelativeTime::seconds (expiresIn);

        if (config_.useRefreshToken)
            saveCredentials();

        callback (true, {});
    });
}

void GoogleAPIManager::refreshAccessToken (AuthCallback callback)
{
    if (credentials_.refreshToken.isEmpty())
    {
        callback (false, "No refresh token available");
        return;
    }

    URL url ("https://oauth2.googleapis.com/token");

    StringPairArray postParams;
    postParams.set ("refresh_token", credentials_.refreshToken);
    postParams.set ("client_id", config_.clientId);
    postParams.set ("client_secret", config_.clientSecret);
    postParams.set ("grant_type", "refresh_token");

    String postData;
    for (int i = 0; i < postParams.size(); ++i)
    {
        if (i > 0)
            postData << "&";

        postData += urlEncode (postParams.getAllKeys()[i]) + "=" + urlEncode (postParams.getAllValues()[i]);
    }

    StringPairArray headers;
    headers.set ("Content-Type", "application/x-www-form-urlencoded");

    makeRequest (url, "POST", postData, headers, [this, callback] (const var& data, const String& error) {
        if (! error.isEmpty())
        {
            callback (false, error);
            return;
        }

        ScopedLock lock (credentialsLock_);
        credentials_.accessToken = data["access_token"].toString();
        credentials_.tokenType = data["token_type"].toString();

        const int expiresIn = data["expires_in"];
        credentials_.expiresAt = Time::getCurrentTime() + RelativeTime::seconds (expiresIn);

        if (config_.useRefreshToken)
            saveCredentials();

        callback (true, {});
    });
}

StringPairArray GoogleAPIManager::getAuthHeaders() const
{
    StringPairArray headers;
    ScopedLock lock (credentialsLock_);
    headers.set ("Authorization", credentials_.tokenType + " " + credentials_.accessToken);
    return headers;
}

String GoogleAPIManager::buildScopesString() const
{
    return config_.scopes.joinIntoString (" ");
}

var GoogleAPIManager::parseJsonResponse (const String& response, String& error) const
{
    const auto data = JSON::parse (response);

    if (! data.isObject())
    {
        error = "Invalid JSON response";
        return var();
    }

    if (data.hasProperty ("error"))
    {
        var errorObj = data["error"];
        if (errorObj.isObject())
            error = errorObj["message"].toString();
        else
            error = errorObj.toString();

        return var();
    }

    return data;
}

String GoogleAPIManager::urlEncode (const String& text) const
{
    return URL::addEscapeChars (text, false);
}

Time GoogleAPIManager::parseRFC3339DateTime (const String& dateTime) const
{
    return Time::fromISO8601 (dateTime);
}

String GoogleAPIManager::formatRFC3339DateTime (const Time& time) const
{
    return time.toISO8601 (true);
}

URL GoogleAPIManager::getSheetsUrl (const String& endpoint) const
{
    return URL ("https://sheets.googleapis.com/v4/" + endpoint);
}

URL GoogleAPIManager::getDocsUrl (const String& endpoint) const
{
    return URL ("https://docs.googleapis.com/v1/" + endpoint);
}

URL GoogleAPIManager::getGmailUrl (const String& endpoint) const
{
    return URL ("https://gmail.googleapis.com/gmail/v1/" + endpoint);
}

URL GoogleAPIManager::getCalendarUrl (const String& endpoint) const
{
    return URL ("https://www.googleapis.com/calendar/v3/" + endpoint);
}

#endif // SQUAREPINE_USE_GOOGLE_API
