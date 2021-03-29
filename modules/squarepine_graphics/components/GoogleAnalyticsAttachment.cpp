GoogleAnalyticsAttachment::GoogleAnalyticsAttachment (const String& t, const String& c, const String& a, const String& l, int v) :
    id (t.trim()),
    category (c.trim()),
    action (a.trim()),
    label (l.trim()),
    value (v)
{
    //These params are not optional and will bugger up the analytics if missing!
    jassert (id.isNotEmpty());
    jassert (category.isNotEmpty());
    jassert (action.isNotEmpty());
}

StringPairArray GoogleAnalyticsAttachment::generateMetadata()
{
    GoogleAnalyticsMetadata metadata (id);

    return metadata.withEventCategory (category)
                   .withEventAction (action)
                   .withEventLabel (label)
                   .withEventValue (value);
}

void GoogleAnalyticsAttachment::buttonClicked (Button* button)
{
    jassert (button != nullptr);

    const bool shouldAutomaticallyLabel = label.isEmpty();

    if (shouldAutomaticallyLabel)
    {
        auto buttonId = button->getName();

        if (buttonId.isEmpty() && button->getCommandID() != 0)
            buttonId = String (button->getCommandID());

        label = buttonId.trim();
    }

    GoogleAnalyticsReporter().sendReport (generateMetadata());

    if (shouldAutomaticallyLabel)
        label = String();
}
