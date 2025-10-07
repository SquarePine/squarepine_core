#if SQUAREPINE_USE_GOOGLE_ANALYTICS

GoogleAnalyticsAttachment::GoogleAnalyticsAttachment (const String& t,
                                                      const String& c,
                                                      const String& a,
                                                      std::optional<String> l,
                                                      std::optional<int> v) :
    id (t.trim()),
    category (c.trim()),
    action (a.trim()),
    label (l),
    value (v)
{
    // These params are not optional and will bugger up the analytics if missing!
    jassert (id.isNotEmpty());
    jassert (category.isNotEmpty());
    jassert (action.isNotEmpty());
}

GoogleAnalyticsAttachment::GoogleAnalyticsAttachment (const String& t,
                                                      const String& c,
                                                      const String& a) :
    GoogleAnalyticsAttachment (t, c, a, std::optional<String> (), {})
{
}

GoogleAnalyticsAttachment::GoogleAnalyticsAttachment (const String& t, const String& c,
                                                      const String& a, const String& l, int v) :
    GoogleAnalyticsAttachment (t, c, a, std::optional<String> (l.trim()), v)
{
}

//==============================================================================
void GoogleAnalyticsAttachment::setEventLabel (std::optional<String> el)
{
    label = el;
}

void GoogleAnalyticsAttachment::setEventValue (std::optional<int> ev)
{
    value = ev;
}

//==============================================================================
StringPairArray GoogleAnalyticsAttachment::generateMetadata()
{
    GoogleAnalyticsMetadata metadata (id);
    metadata.withEventCategory (category)
            .withEventAction (action);

    if (label.has_value()) metadata.withEventLabel (*label);
    if (value.has_value()) metadata.withEventValue (*value);

    return metadata;
}

String GoogleAnalyticsAttachment::generateLabel (Button& button) const
{
    auto buttonId = button.getName();

    if (buttonId.isEmpty() && button.getCommandID() != 0)
        buttonId = String (button.getCommandID());

    return buttonId.trim();
}

int GoogleAnalyticsAttachment::generateValue (Button& button) const
{
    jassert (button.isToggleable());
    return button.getToggleState() ? 1 : 0;
}

//==============================================================================
void GoogleAnalyticsAttachment::buttonClicked (Button* button)
{
    jassert (button != nullptr);

    const bool shouldAutoLabel = ! label.has_value();
    if (shouldAutoLabel)
        label = generateLabel (*button);

    const bool shouldAutoFindValue = ! value.has_value() && button->isToggleable();
    if (shouldAutoFindValue)
        value = generateValue (*button);

    SharedResourcePointer<sp::GoogleAnalyticsReporter> googleAnalyticsReporter;
    googleAnalyticsReporter->sendReport (generateMetadata());

    if (shouldAutoLabel)
        label = {};

    if (shouldAutoFindValue)
        value = {};
}

#endif // SQUAREPINE_USE_GOOGLE_ANALYTICS
