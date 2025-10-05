//==============================================================================
/** Example class demonstrating GoogleAPIManager usage */
class GoogleAPIDemo final : public DemoBase
{
public:
    /** */
    GoogleAPIDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("Google API Manager"))
    {
        setupUI();
        setupGoogleAPI();
    }

    //==============================================================================
    void resized() override
    {
        auto area = getLocalBounds().reduced (dims::marginPx);

        statusLabel.setBounds (area.removeFromTop (30));
        area.removeFromTop (10);

        authCodeEditor.setBounds (area.removeFromTop (30));
        area.removeFromTop (5);
        authButton.setBounds (area.removeFromTop (30));
        area.removeFromTop (10);

        const int buttonHeight = 30;
        const int buttonSpacing = 5;

        sheetsButton.setBounds (area.removeFromTop (buttonHeight));
        area.removeFromTop (buttonSpacing);
        docsButton.setBounds (area.removeFromTop (buttonHeight));
        area.removeFromTop (buttonSpacing);
        gmailButton.setBounds (area.removeFromTop (buttonHeight));
        area.removeFromTop (buttonSpacing);
        calendarButton.setBounds (area.removeFromTop (buttonHeight));
    }

private:
    //==============================================================================
    GoogleAPIManager apiManager;

    // UI Components
    Label statusLabel;
    TextEditor authCodeEditor;
    TextButton authButton { "Authenticate with Code" };
    TextButton sheetsButton { "Test Sheets API" };
    TextButton docsButton { "Test Docs API" };
    TextButton gmailButton { "Test Gmail API" };
    TextButton calendarButton { "Test Calendar API" };

    //==============================================================================
    void setupGoogleAPI()
    {
        // Set your OAuth2 client credentials (get these from Google Cloud Console)
        apiManager.setClientCredentials (
            "your_client_id_here.apps.googleusercontent.com",
            "your_client_secret_here"
        );

        // Add the scopes you need
        apiManager.addScope (GoogleAPIManager::Scopes::SHEETS_READWRITE);
        apiManager.addScope (GoogleAPIManager::Scopes::DOCS_READWRITE);
        apiManager.addScope (GoogleAPIManager::Scopes::GMAIL_READWRITE);
        apiManager.addScope (GoogleAPIManager::Scopes::CALENDAR_READWRITE);

        // Try to authenticate (will load saved credentials if available)
        apiManager.authenticate ([this] (bool success, const String& error) {
            if (success)
            {
                Logger::writeToLog ("Authentication successful!");
                statusLabel.setText ("Authenticated successfully", dontSendNotification);
                enableButtons (true);
            }
            else
            {
                Logger::writeToLog ("Authentication failed: " + error);
                statusLabel.setText ("Authentication required", dontSendNotification);

                // Show authorisation URL for manual authentication
                const String authUrl = apiManager.getAuthorizationUrl();
                Logger::writeToLog ("Please visit this URL to authorise: " + authUrl);

                // In a real app, you might open this URL in a browser
                // URL::launchInDefaultBrowser (authUrl);
            }
        });
    }

    void authenticateWithCode (const String& authCode)
    {
        apiManager.authenticateWithCode (authCode, [this] (bool success, const String& error) {
            if (success)
            {
                Logger::writeToLog ("Authentication with code successful!");
                statusLabel.setText ("Authenticated successfully", dontSendNotification);
                enableButtons (true);
            }
            else
            {
                Logger::writeToLog ("Authentication with code failed: " + error);
                statusLabel.setText ("Authentication failed: " + error, dontSendNotification);
            }
        });
    }

    void demonstrateSheets()
    {
        Logger::writeToLog ("=== Google Sheets API Demo ===");

        // Create a new spreadsheet
        apiManager.createSpreadsheet ("Test Spreadsheet from JUCE", [this] (const var& data, const String& error) {
            if (! error.isEmpty())
            {
                Logger::writeToLog ("Error creating spreadsheet: " + error);
                return;
            }

            const String spreadsheetId = data["spreadsheetId"].toString();
            Logger::writeToLog ("Created spreadsheet with ID: " + spreadsheetId);

            // Add some data to the spreadsheet
            var values;
            var row1, row2, row3;
            row1.append ("Name");
            row1.append ("Age");
            row1.append ("City");
            row2.append ("John");
            row2.append (25);
            row2.append ("New York");
            row3.append ("Jane");
            row3.append (30);
            row3.append ("London");

            values.append (row1);
            values.append (row2);
            values.append (row3);

            apiManager.updateSheetsData (spreadsheetId, "Sheet1!A1:C3", values, [spreadsheetId] (bool success, const String& error) {
                if (success)
                {
                    Logger::writeToLog ("Successfully added data to spreadsheet");

                    // Now read the data back
                    // Note: 'this' capture might be invalid here in real usage
                    // You should use weak references or ensure lifetime
                }
                else
                {
                    Logger::writeToLog ("Error adding data: " + error);
                }
            });
        });
    }

    void demonstrateDocs()
    {
        Logger::writeToLog ("=== Google Docs API Demo ===");

        // Create a new document
        apiManager.createDocument ("Test Document from JUCE", [this] (const var& data, const String& error) {
            if (! error.isEmpty())
            {
                Logger::writeToLog ("Error creating document: " + error);
                return;
            }

            const String documentId = data["documentId"].toString();
            Logger::writeToLog ("Created document with ID: " + documentId);

            // Insert some text
            apiManager.insertText (documentId, 1, "Hello from JUCE!\n\nThis document was created using the GoogleAPIManager class.",
                [documentId] (bool success, const String& error) {
                    if (success)
                        Logger::writeToLog ("Successfully inserted text into document");
                    else
                        Logger::writeToLog ("Error inserting text: " + error);
                });
        });
    }

    void demonstrateGmail()
    {
        Logger::writeToLog ("=== Gmail API Demo ===");

        // Get user profile
        apiManager.getProfile ([this] (const var& data, const String& error) {
            if (! error.isEmpty())
            {
                Logger::writeToLog ("Error getting profile: " + error);
                return;
            }

            Logger::writeToLog ("Gmail profile: " + JSON::toString (data));

            // Get recent messages
            apiManager.getMessages ("", 5, [] (const var& data, const String& error) {
                if (! error.isEmpty())
                {
                    Logger::writeToLog ("Error getting messages: " + error);
                    return;
                }

                Logger::writeToLog ("Recent messages: " + JSON::toString (data));
            });
        });

        // Send a test email (commented out to avoid spam)
        /*
        apiManager.sendEmail (
            "recipient@example.com",
            "Test Email from JUCE",
            "Hello!\n\nThis email was sent using the GoogleAPIManager class in JUCE.\n\nBest regards,\nYour JUCE App",
            "",
            [] (bool success, const String& error) {
                if (success)
                    Logger::writeToLog ("Email sent successfully!");
                else
                    Logger::writeToLog ("Error sending email: " + error);
            }
        );
        */
    }

    void demonstrateCalendar()
    {
        Logger::writeToLog ("=== Google Calendar API Demo ===");

        // Get calendar list
        apiManager.getCalendars ([this] (const var& data, const String& error) {
            if (! error.isEmpty())
            {
                Logger::writeToLog ("Error getting calendars: " + error);
                return;
            }

            Logger::writeToLog ("Calendars: " + JSON::toString (data));

            // Create a test event
            auto event = new DynamicObject();
            event->setProperty ("summary", "Test Event from JUCE");
            event->setProperty ("description", "This event was created using GoogleAPIManager");

            // Set start time (1 hour from now)
            auto startTime = new DynamicObject();
            const Time start = Time::getCurrentTime() + RelativeTime::hours (1);
            startTime->setProperty ("dateTime", start.toISO8601 (true));
            startTime->setProperty ("timeZone", "UTC");
            event->setProperty ("start", var (startTime));

            // Set end time (2 hours from now)
            auto endTime = new DynamicObject();
            const Time end = Time::getCurrentTime() + RelativeTime::hours (2);
            endTime->setProperty ("dateTime", end.toISO8601 (true));
            endTime->setProperty ("timeZone", "UTC");
            event->setProperty ("end", var (endTime));

            apiManager.createEvent ("primary", var (event), [] (const var& data, const String& error) {
                if (! error.isEmpty())
                {
                    Logger::writeToLog ("Error creating event: " + error);
                    return;
                }

                Logger::writeToLog ("Created event: " + JSON::toString (data));
            });
        });
    }

    void setupUI()
    {
        addAndMakeVisible (statusLabel);
        statusLabel.setText ("Initialising...", dontSendNotification);
        statusLabel.setJustificationType (Justification::centred);

        addAndMakeVisible (authCodeEditor);
        authCodeEditor.setTextToShowWhenEmpty ("Paste authorisation code here", Colours::grey);

        addAndMakeVisible (authButton);
        authButton.onClick = [this]() {
            const String code = authCodeEditor.getText().trim();
            if (code.isNotEmpty())
            {
                authenticateWithCode (code);
                authCodeEditor.clear();
            }
        };

        addAndMakeVisible (sheetsButton);
        sheetsButton.onClick = [this]() { demonstrateSheets(); };

        addAndMakeVisible (docsButton);
        docsButton.onClick = [this]() { demonstrateDocs(); };

        addAndMakeVisible (gmailButton);
        gmailButton.onClick = [this]() { demonstrateGmail(); };

        addAndMakeVisible (calendarButton);
        calendarButton.onClick = [this]() { demonstrateCalendar(); };

        enableButtons (false);
    }

    void enableButtons (bool enabled)
    {
        sheetsButton.setEnabled (enabled);
        docsButton.setEnabled (enabled);
        gmailButton.setEnabled (enabled);
        calendarButton.setEnabled (enabled);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoogleAPIDemo)
};
