/** A simple Google Analytics attachment that fires off an event
    when a button is clicked.

    @see GoogleAnalyticsMetadata, GoogleAnalyticsReporter
*/
class GoogleAnalyticsAttachment : public Button::Listener
{
public:
    /** Constructor

        @param trackingId   The 'tid', or tracking ID.
        @param category     The 'ec', or event category.
        @param action       The 'ea', or event action.
        @param label        The 'el', or event label. This parameter is optional.
        @param value        The 'ev', or even value. This parameter is optional.
    */
    GoogleAnalyticsAttachment (const String& trackingId, const String& category, const String& action,
                               const String& label = String(), int value = 0);

    /** @returns the tracking ID used in conjunction with Google Analytics. */
    const String& getTrackingId() const noexcept { return id; }

    /** @returns the event category for when the button is clicked. */
    const String& getEventCategory() const noexcept { return category; }

    /** @returns the event action for when the button is clicked. */
    const String& getEventAction() const noexcept { return action; }

    /** @returns the event label for when the button is clicked. */
    const String& getEventLabel() const noexcept { return label; }

    /** @returns the event value for when the button is clicked. */
    int getEventValue() const noexcept { return value; }

    //==============================================================================
    /** @internal */
    void buttonClicked (Button*) override;

protected:
    //==============================================================================
    /** Generates a typical group of event related metadata.

        You may override this if you want to further extend or customise the information
        that will be reported.
    */
    virtual StringPairArray generateMetadata();

private:
    //==============================================================================
    const String id;
    String category, action, label;
    int value = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoogleAnalyticsAttachment)
};
