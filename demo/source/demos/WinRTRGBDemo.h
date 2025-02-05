#if SQUAREPINE_USE_WINRTRGB

/** */
class WinRTRGBDemo final : public DemoBase,
                           public ListBoxModel,
                           private Timer
{
public:
    /** */
    WinRTRGBDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("WinRT RGB"))
    {
        auto setupButton = [&] (TextButton& tb, StringRef text, Colour c)
        {
            constexpr auto id = TextButton::ColourIds::buttonColourId;

            tb.setButtonText (text);
            tb.setColour (id, c);
            tb.onClick = [c]()
            {
            };

            addAndMakeVisible (tb);
        };

        setupButton (redButton, TRANS ("Red"), Colours::red);
        setupButton (greenButton, TRANS ("Green"), Colours::green);
        setupButton (blueButton, TRANS ("Blue"), Colours::blue);

        listbox.setRowHeight (barSizePx);
        listbox.setModel (this);
        addAndMakeVisible (listbox);

        startTimerHz (60);
    }

    //==============================================================================
    void resized() override
    {
        auto b = getLocalBounds().reduced (marginPx);

        {
            auto lb = b.removeFromTop (jmin (b.getHeight(), (int) barSizePx));
            auto w = (lb.getWidth() / 3) - (marginPx * 2);

            redButton.setBounds (lb.removeFromLeft (w));
            lb.removeFromLeft (marginPx);

            blueButton.setBounds (lb.removeFromRight (w));
            lb.removeFromRight (marginPx);

            greenButton.setBounds (lb);
        }

        b.removeFromTop (marginPx);
        listbox.setBounds (b);
    }

    //==============================================================================
    void timerCallback() override
    {
        auto newDevices = sp::WinRTRGB().getConnectedDevices();
        bool changed = connectedDevices.size() != newDevices.size();
        if (! changed)
        {
            
        }

        if (changed)
        {
            connectedDevices.swapWith (newDevices);
            listbox.updateContent();
        }

        repaint(); // For the LEDs.
    }

    //==============================================================================
    int getNumRows() override { return 0; }

    void paintListBoxItem (int, Graphics&, int, int, bool) override
    {
    }

private:
    //==============================================================================
    enum
    {
        pxW = 2,
        pxH = 2
    };

    //==============================================================================
    TextButton redButton, greenButton, blueButton;
    ListBox listbox;
    Array<WinRTRGB::Device> connectedDevices;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WinRTRGBDemo)
};

#endif // SQUAREPINE_USE_WINRTRGB
