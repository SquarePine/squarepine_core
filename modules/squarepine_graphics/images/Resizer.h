//==============================================================================
/** If AVIR is available, this really smoothly resizes
    your image to the destination dimensions.

    Otherwise, it'll use JUCE's resizing method.
*/
Image applyResize (const Image&, int destinationWidth, int destinationHeight);

/** If AVIR is available, this really smoothly resizes
    your image to the destination scale.

    Otherwise, it'll use JUCE's resizing method.
*/
Image applyResize (const Image&, float scale);

//==============================================================================
/** A component that simply displays an image.

    Use setImage to give it an image, and it'll display it - simple as that!

    When enabled, this will use AVIR on Intel systems to resize the image.
    It's a much higher quality result at the expense of a few more CPU cycles.
*/
class HighQualityImageComponent final : public Component,
                                        public SettableTooltipClient
{
public:
    //==============================================================================
    /** Creates an HighQualityImageComponent. */
    HighQualityImageComponent (const String& componentName = String());

    //==============================================================================
    /** Sets the image that should be displayed. */
    void setImage (const Image&);

    /** Sets the image that should be displayed, and its placement within the component. */
    void setImage (const Image&, RectanglePlacement);

    /** @returns the current image. */
    const Image& getImage() const noexcept { return image; }

    /** Sets the method of positioning that will be used to fit the image within the component's bounds.
        By default the positioning is centred, and will fit the image inside the component's bounds
        whilst keeping its aspect ratio correct, but you can change it to whatever layout you need.
    */
    void setImagePlacement (RectanglePlacement);

    /** @returns the current image placement. */
    const RectanglePlacement& getImagePlacement() const noexcept { return placement; }

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    void paint (Graphics&) override;

private:
    //==============================================================================
    Image image;
    RectanglePlacement placement = RectanglePlacement::centred;

    Image resizedImage;
    juce::Rectangle<int> lastKnownBounds;

    //==============================================================================
    std::unique_ptr<AccessibilityHandler> createAccessibilityHandler() override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HighQualityImageComponent)
};
