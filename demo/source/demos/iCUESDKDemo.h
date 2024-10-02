#if SQUAREPINE_USE_ICUESDK

/** */
class CueSDKDemo final : public DemoBase,
                         public ListBoxModel,
                         private Timer
{
public:
    /** */
    CueSDKDemo (SharedObjects& sharedObjs) :
        DemoBase (sharedObjs, NEEDS_TRANS ("iCUESDK Demo"))
    {
        auto setupButton = [&] (TextButton& tb, StringRef text, Colour c)
        {
            constexpr auto id = TextButton::ColourIds::buttonColourId;

            tb.setButtonText (text);
            tb.setColour (id, c);
            tb.onClick = [c]()
            {
                corsair::updateAllLEDsWithColour (c);
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

    void timerCallback() override
    {
        const auto newDevs = corsair::getAllAvailableDevices();

        bool changed = false;

        // Remove disconnected devices:
        for (int i = devices.size(); --i >= 0;)
        {
            auto* device = devices.getUnchecked (i);
            const String id (device->deviceInfo.id);

            bool found = false;
            for (const auto& newDevice : newDevs)
            {
                if (id == String (newDevice.id))
                {
                    found = true;
                    break;
                }
            }

            if (! found)
            {
                device = nullptr;
                devices.remove (i);
                changed = true;
            }
        }

        // Add new devices:
        for (const auto& newDevice : newDevs)
        {
            const String id (newDevice.id);

            bool found = false;
            for (const auto& originalDevice : devices)
            {
                if (id == String (originalDevice->deviceInfo.id))
                {
                    found = true;
                    break;
                }
            }

            if (! found)
            {
                auto* ledData = devices.add (new LEDData());
                ledData->deviceInfo = newDevice;
                ledData->ledPositions = corsair::getLEDIDs (ledData->deviceInfo);
                changed = true;
            }
        }

        if (changed)
            listbox.updateContent();

        repaint(); // For the LEDs.
    }

    int getNumRows() override { return devices.size(); }

    void paintListBoxItem (int rowIndex, Graphics& g, int width, int height, bool) override
    {
        auto* device = devices[rowIndex];
        if (device == nullptr || width <= 1 || height <= 1)
            return;

        juce::Rectangle<int> area (width, height);
        area = area.reduced (marginPx);

        g.setColour (Colours::white);

        const auto text = [&]()
        {
            auto s = String (device->deviceInfo.model);
            s << newLine << corsair::toString (device->deviceInfo.type);

            if (device->isConnected())
                s << newLine << "serial: " + String (device->deviceInfo.serial);

            return s;
        }();

        g.drawFittedText (text, area.removeFromLeft (128 * 3), Justification::centredLeft, 3, 1.0f);

        if (device->isConnected())
        {
            device->ledColours = corsair::getLEDColours (device->deviceInfo.id, device->ledPositions);
            if (device->ledColours.isEmpty())
                return;

            const auto numPx    = device->ledColours.size();
            const auto pxPerRow = area.getWidth() / (pxW * 2); // the *2 is for the tiny margin
            const auto numRows  = (int) std::ceil ((double) numPx / (double) pxPerRow);

            for (int i = 0; i < numPx; ++i)
            {
                auto column = area.removeFromLeft (pxW);

                for (int f = 0; f < numRows; ++f)
                {
                    const auto colourIndex = (i % pxPerRow) + (f * pxPerRow);

                    if (colourIndex >= numPx)
                        break;

                    g.setColour (device->getColourUnchecked (colourIndex));

                    g.fillRect (column.removeFromTop (pxH));
                    column.removeFromTop (pxH); // Tiny row margin
                }

                // Tiny column margin:
                area.removeFromLeft (pxW);
            }
        }
    }

private:
    enum
    {
        pxW         = 2,
        pxH         = 2,
        marginPx    = 4,
        barSizePx   = marginPx * 12
    };

    struct LEDData final
    {
        bool isConnected() const
        {
            return corsair::isConnected (deviceInfo);
        }

        Colour getColourUnchecked (int index) const
        {
            return corsair::toColour (ledColours.getReference (index));
        }

        corsair::CorsairDeviceInfo deviceInfo;
        Array<corsair::CorsairLedPosition> ledPositions;
        Array<corsair::CorsairLedColor> ledColours;
    };

    OwnedArray<LEDData> devices;
    TextButton redButton, greenButton, blueButton;
    ListBox listbox;

    Grid pixelGrid;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CueSDKDemo)
};

#endif // SQUAREPINE_USE_ICUESDK
