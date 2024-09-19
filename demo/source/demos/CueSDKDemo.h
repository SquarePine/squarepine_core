#if SQUAREPINE_USE_CUESDK

/** */
class CueSDKDemo final : public DemoBase
{
public:
    /** */
    CueSDKDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("CueSDK Demo"))
    {
        auto setupButton = [&] (TextButton& tb, StringRef text, Colour c)
        {
            constexpr auto id = TextButton::ColourIds::buttonColourId;

            tb.setButtonText (text);
            tb.setColour (id, c);
            tb.onClick = [c]()
            {
                sp::corsair::updateAllLEDsWithColour (c);
            };

            addAndMakeVisible (tb);
        };

        setupButton (redButton, TRANS ("Red"), Colours::red);
        setupButton (greenButton, TRANS ("Green"), Colours::green);
        setupButton (blueButton, TRANS ("Blue"), Colours::blue);
    }

    void resized() override
    {
        constexpr auto marginPx = 4;
        constexpr auto barSizePx = marginPx * 8;

        auto b = getLocalBounds().reduced (4);
        b = b.withHeight (jmin (b.getHeight(), barSizePx));
        auto w = (b.getWidth() / 3) - (marginPx * 2);

        redButton.setBounds (b.removeFromLeft (w));
        b.removeFromLeft (marginPx);

        blueButton.setBounds (b.removeFromRight (w));
        b.removeFromRight (marginPx);

        greenButton.setBounds (b);
    }

private:
    TextButton redButton, greenButton, blueButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueSDKDemo)
};

#endif // SQUAREPINE_USE_CUESDK
