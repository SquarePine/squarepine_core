#if SQUAREPINE_USE_GOOGLE_API

//==============================================================================
/** A comprehensive Google API manager that provides access to Google's core services
    including Sheets, Docs, Gmail, and Calendar APIs.

    This class handles OAuth2 authentication, HTTP requests, and JSON parsing
    using JUCE's built-in networking and data structures.

    Example usage:
    @code
    sp::GoogleAPIManager apiManager;
    apiManager.setClientCredentials ("your_client_id", "your_client_secret");
    apiManager.authenticate ([this] (bool success) {
        if (success) {
            // Use the API
            apiManager.getSheetsData ("spreadsheet_id", "Sheet1!A1:B10",
                [] (const var& data, const String& error) {
                    if (error.isEmpty())
                        DBG ("Got data: " + JSON::toString (data));
                });
        }
    });
    @endcode
*/
class GoogleAPIManager final
{
public:
    //==============================================================================
    /** Callback types for async operations */
    using AuthCallback      = std::function<void (Result result)>;
    using DataCallback      = std::function<void (const var& data, Result result)>;
    using StringCallback    = std::function<void (const String& data, Result result)>;
    using BoolCallback      = std::function<void (Result result)>;

    //==============================================================================
    /** OAuth2 scopes for different Google services */
    struct Scopes
    {
        static const String SHEETS_READWRITE;
        static const String DOCS_READWRITE;
        static const String GMAIL_READWRITE;
        static const String CALENDAR_READWRITE;
        static const String USERINFO_EMAIL;
        static const String USERINFO_PROFILE;
    };

    //==============================================================================
    /** Configuration structure for API settings */
    struct Config
    {
        String clientId,
               clientSecret,
               redirectUri = "urn:ietf:wg:oauth:2.0:oob"; // For installed apps
        StringArray scopes;
        int timeoutMs = 30000;
        bool useRefreshToken = true;
        String credentialsFile = "google_credentials.json";
    };

    //==============================================================================
    GoogleAPIManager() = default;

    //==============================================================================
    /** Initialise with client credentials */
    void setClientCredentials (const String& clientId, const String& clientSecret);

    /** Set configuration */
    void setConfig (const Config&);

    /** Add OAuth2 scope */
    void addScope (const String&);

    /** Set custom redirect URI */
    void setRedirectUri (const String&);

    //==============================================================================
    /** Authentication methods */
    void authenticate (AuthCallback);
    void authenticateWithCode (const String& authCode, AuthCallback);
    String getAuthorizationUrl();
    bool isAuthenticated() const;
    void clearCredentials();

    //==============================================================================
    /** Google Sheets API */

    /** Get data from a spreadsheet range */
    void getSheetsData (const String& spreadsheetId, const String& range, DataCallback);

    /** Update data in a spreadsheet range */
    void updateSheetsData (const String& spreadsheetId, const String& range,
                           const var& values, BoolCallback callback);

    /** Append data to a spreadsheet */
    void appendSheetsData (const String& spreadsheetId, const String& range,
                           const var& values, BoolCallback);

    /** Create a new spreadsheet */
    void createSpreadsheet (const String& title, DataCallback);

    /** Get spreadsheet metadata */
    void getSpreadsheetInfo (const String& spreadsheetId, DataCallback);

    //==============================================================================
    /** Google Docs API */

    /** Get document content */
    void getDocument (const String& documentId, DataCallback);

    /** Create a new document */
    void createDocument (const String& title, DataCallback);

    /** Update document content */
    void updateDocument (const String& documentId, const var& requests, BoolCallback);

    /** Insert text at specific location */
    void insertText (const String& documentId, int index, const String& text, BoolCallback);

    //==============================================================================
    /** Gmail API */

    /** Get list of messages */
    void getMessages (const String& query = {}, int maxResults = 10, DataCallback callback = nullptr);

    /** Get specific message */
    void getMessage (const String& messageId, DataCallback);

    /** Send email */
    void sendEmail (const String& to, const String& subject, const String& body,
                    const String& from = {}, BoolCallback callback = nullptr);

    /** Get user profile */
    void getProfile (DataCallback callback);

    /** Get labels */
    void getLabels (DataCallback callback);

    //==============================================================================
    /** Google Calendar API */

    /** Get calendar list */
    void getCalendars (DataCallback callback);

    /** Get events from calendar */
    void getEvents (const String& calendarId = "primary",
                    const String& timeMin = {}, const String& timeMax = {},
                    DataCallback callback = nullptr);

    /** Create event */
    void createEvent (const String& calendarId, const var& event, DataCallback callback = nullptr);

    /** Update event */
    void updateEvent (const String& calendarId, const String& eventId,
                      const var& event, BoolCallback callback = nullptr);

    /** Delete event */
    void deleteEvent (const String& calendarId, const String& eventId, BoolCallback callback = nullptr);

    //==============================================================================
    /** Get current access token */
    String getAccessToken() const;

    /** Check if token needs refresh */
    bool needsTokenRefresh() const;

    /** Manually refresh token */
    void refreshToken (AuthCallback callback);

    /** Save credentials to file */
    void saveCredentials();

    /** Load credentials from file */
    bool loadCredentials();

private:
    //==============================================================================
    /** Internal structures */
    struct Credentials
    {
        String accessToken,
               refreshToken,
               tokenType = "Bearer";
        Time expiresAt;
        StringArray scopes;
    };

    //==============================================================================
    /** Member variables */
    Config config_;
    Credentials credentials_;
    CriticalSection credentialsLock_;

    //==============================================================================
    /** Internal HTTP methods */
    void makeRequest (URL url, const String& method, const String& postData,
                      const String& headers, DataCallback callback);
    void makeRequest (URL url, const String& method, const String& postData,
                      const StringPairArray& headers, DataCallback callback);

    void makeAuthenticatedRequest (URL url, const String& method,
                                   const String& postData, DataCallback callback);

    //==============================================================================
    /** OAuth2 implementation */
    void exchangeCodeForToken (const String& code, AuthCallback callback);
    void refreshAccessToken (AuthCallback callback);

    //==============================================================================
    /** Utility methods */
    StringPairArray getAuthHeaders() const;
    String buildScopesString() const;
    var parseJsonResponse (const String& response, String& error) const;
    String urlEncode (const String& text) const;
    Time parseRFC3339DateTime (const String& dateTime) const;
    String formatRFC3339DateTime (const Time& time) const;

    //==============================================================================
    /** API endpoint helpers */
    URL getSheetsUrl (const String& endpoint) const;
    URL getDocsUrl (const String& endpoint) const;
    URL getGmailUrl (const String& endpoint) const;
    URL getCalendarUrl (const String& endpoint) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoogleAPIManager)
};

#endif // SQUAREPINE_USE_GOOGLE_API
