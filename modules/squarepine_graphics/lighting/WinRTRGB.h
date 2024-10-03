#if SQUAREPINE_USE_WINRTRGB

/** */
class WinRTRGB final
{
public:
    /** */
    WinRTRGB();
    /** */
    ~WinRTRGB();

    //==============================================================================
    /** */
    struct Device final
    {
        enum Purpose
        {
            undefined       = 0,
            control         = 1 << 0,
            accent          = 1 << 1,
            branding        = 1 << 2,
            status          = 1 << 3,
            illumination    = 1 << 4,
            presentation    = 1 << 5
        };

        int index = 0;
        Colour colour = Colours::transparentBlack;
        Vector3D<float> position;
        Purpose purpose = undefined;
    };

    /** */
    [[nodiscard]] Array<Device> getConnectedDevices() const;

private:
    //==============================================================================
    class Pimpl;
    std::unique_ptr<Pimpl> pimpl;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WinRTRGB)
};

#endif // SQUAREPINE_USE_WINRTRGB
