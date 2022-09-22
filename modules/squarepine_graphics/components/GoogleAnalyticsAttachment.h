#if SQUAREPINE_USE_GOOGLE_ANALYTICS

/** A simple Google Analytics attachment that fires off an event
    when a button is clicked.

    This will use a SharedResourcePointer<sp::GoogleAnalyticsReporter>
    to fetch the reporter.

    Some extra details:
    - The event category is the name assigned to the group of similar
      events you want to track.
    - The event action is the type of event you want to track for a particular button.
    - The event label is the name assigned to the button whose interactions you want to track.
      This can the name of a file, etc...
    - The event value is the numerical value assigned to the event you want to track.

    @see GoogleAnalyticsMetadata, GoogleAnalyticsReporter
*/
class GoogleAnalyticsAttachment : public Button::Listener
{
public:
    /** Constructor which will use the button you're listening for
        to generate a label and value automatically.

        This will use the button name or command ID,
        whichever is present (the name first),
        to dynamically set up the label.

        For a toggle button, this will dynamically
        set the value to 1 or 0 depending on the toggle state.

        @param trackingId   The 'tid', or tracking ID.
        @param category     The 'ec', or event category.
        @param action       The 'ea', or event action.
    */
    GoogleAnalyticsAttachment (const String& trackingId,
                               const String& category,
                               const String& action);

    /** Constructor.

        @param trackingId   The 'tid', or tracking ID.
        @param category     The 'ec', or event category.
        @param action       The 'ea', or event action.
        @param label        The 'el', or event label. This parameter is optional.
        @param value        The 'ev', or event value. This parameter is optional.
    */
    GoogleAnalyticsAttachment (const String& trackingId,
                               const String& category,
                               const String& action,
                               const String& label,
                               int value);

    //==============================================================================
    /** @returns the tracking ID used in conjunction with Google Analytics. */
    const String& getTrackingId() const noexcept { return id; }

    /** @returns the event category for when the button is clicked. */
    const String& getEventCategory() const noexcept { return category; }

    /** @returns the event action for when the button is clicked. */
    const String& getEventAction() const noexcept { return action; }

    //==============================================================================
    /** Allows dynamically changing the event label. */
    void setEventLabel (std::optional<String>);

    /** @returns the event label for when the button is clicked.
        This only matters if you set it initially via the constructor
        or by the relevant setter.
    */
    std::optional<String> getEventLabel() const noexcept { return label; }

    /** Allows dynamically changing the event value. */
    void setEventValue (std::optional<int>);

    /** @returns the event value for when the button is clicked.
        This only matters if you set it initially via the constructor
        or by the relevant setter.
    */
    std::optional<int> getEventValue() const noexcept { return value; }

    //==============================================================================
    /** @internal */
    void buttonClicked (Button*) override;

protected:
    //==============================================================================
    /** Generates a typical group of event related metadata.

        You may override this if you want to further extend
        or customise the information that will be reported.
    */
    virtual StringPairArray generateMetadata();

    /** */
    String generateLabel (Button&) const;

    /** */
    int generateValue (Button&) const;

private:
    //==============================================================================
    const String id;
    String category, action;
    std::optional<String> label;
    std::optional<int> value;

    //==============================================================================
    GoogleAnalyticsAttachment (const String& trackingId,
                               const String& category,
                               const String& action,
                               std::optional<String> label,
                               std::optional<int> value);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoogleAnalyticsAttachment)
};

#endif // SQUAREPINE_USE_GOOGLE_ANALYTICS
