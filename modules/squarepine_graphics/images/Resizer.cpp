#if SQUAREPINE_USE_AVIR_RESIZER

    //==============================================================================
    JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4267 4127 4244 4996 4100 4701 4702 4013
                                     4133 4206 4305 4189 4706 4995 4365 4456
                                     4457 4459 6297 6011 6001 6308 6255 6386
                                     6385 6246 6387 6263 6262 28182)

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wall",
                                         "-Wextra-semi",
                                         "-Wconversion",
                                         "-Wshadow",
                                         "-Wunused-parameter",
                                         "-Wfloat-conversion",
                                         "-Wfloat-equal",
                                         "-Wdeprecated-register",
                                         "-Wdeprecated-declarations",
                                         "-Wswitch-enum",
                                         "-Wzero-as-null-pointer-constant",
                                         "-Wsign-conversion",
                                         "-Wswitch-default",
                                         "-Wredundant-decls",
                                         "-Wmisleading-indentation",
                                         "-Wmissing-prototypes",
                                         "-Wcast-align")

    #include "avir/avir.h"

    #define AVIR_USE_SSE (JUCE_INTEL || __SSE__ || __SSE2__ || __SSE3__)
    #define AVIR_USE_AVX (__AVX__ || __AVX2__)

   #if AVIR_USE_AVX
    #include "avir/avir_float8_avx.h"
   #endif

   #if AVIR_USE_SSE
    #include "avir/avir_float4_sse.h"
   #endif

    JUCE_END_IGNORE_WARNINGS_MSVC
    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

    //==============================================================================
    Image applyResize (const Image& image, int width, int height)
    {
        if (image.isNull() || image.getBounds().isEmpty()
            || width <= 0 || height <= 0)
            return {};

        int channels = 0;

        switch (image.getFormat())
        {
            case Image::ARGB:           channels = 4; break;
            case Image::RGB:            channels = 3; break;
            case Image::SingleChannel:  channels = 1; break;

            default:
                jassertfalse;
                return {};
        };

        Image dest (image.getFormat(), width, height, true);
        Image::BitmapData sourceData (image, Image::BitmapData::readOnly);

        HeapBlock<uint8_t> sourcePacked (image.getWidth() * image.getHeight() * channels);
        auto* rawSourceData = sourcePacked.getData();

        for (int y = 0; y < image.getHeight(); ++y)
            std::memcpy (rawSourceData + y * image.getWidth() * channels,
                         sourceData.getLinePointer (y),
                         (size_t) (image.getWidth() * channels));

       #if AVIR_USE_AVX
        using fpclass_float8 = avir::fpclass_def<avir::float8, float>;
        avir::CImageResizer<fpclass_float8> imageResizer;
       #elif AVIR_USE_SSE
        avir::CImageResizer<avir::fpclass_float4> imageResizer;
       #else
        avir::CImageResizer<> imageResizer;
       #endif

        HeapBlock<uint8_t> destPacked (dest.getWidth() * dest.getHeight() * channels);
        auto* rawDestData = destPacked.getData();

        imageResizer.resizeImage (rawSourceData, image.getWidth(), image.getHeight(), 0,
                                  rawDestData, dest.getWidth(), dest.getHeight(), channels, 0);

        Image::BitmapData destData (dest, Image::BitmapData::readWrite);
        for (int y = 0; y < dest.getHeight(); ++y)
            std::memcpy (destData.getLinePointer (y),
                         rawDestData + y * dest.getWidth() * channels,
                         (size_t) (dest.getWidth() * channels));

        return dest;
    }

#else
    //==============================================================================
    Image applyResize (const Image& source, int width, int height)
    {
        Image dest (Image::ARGB, width, height, true);
        {
            Graphics g (dest);
            g.drawImage (source, juce::Rectangle<int> (width, height).toFloat());
        }

        return dest;
    }

#endif

Image applyResize (const Image& image, float factor)
{
    jassert (factor > 0.0f);

    return applyResize (image,
                        roundToIntAccurate (factor * (float) image.getWidth()),
                        roundToIntAccurate (factor * (float) image.getHeight()));
}

//==============================================================================
HighQualityImageComponent::HighQualityImageComponent (const String& name) :
    Component (name)
{
}

//==============================================================================
void HighQualityImageComponent::setImage (const Image& newImage)
{
    if (image != newImage)
    {
        image = newImage;
        resized();
    }
}

void HighQualityImageComponent::setImage (const Image& newImage, RectanglePlacement rp)
{
    if (image != newImage || placement != rp)
    {
        image = newImage;
        placement = rp;
        resized();
    }
}

void HighQualityImageComponent::setImagePlacement (RectanglePlacement np)
{
    if (placement != np)
    {
        placement = np;
        resized();
    }
}

void HighQualityImageComponent::resized()
{
    const auto b = getLocalBounds();
    if (b.isEmpty() || image.isNull())
        return;

    if (lastKnownBounds != b)
    {
        // NB: Must scale uniformly and let the placement drive the rest:
        resizedImage = applyResize (image, (float) b.getHeight() / (float) image.getHeight());
        lastKnownBounds = b;
    }
}

void HighQualityImageComponent::paint (Graphics& g)
{
    const auto b = getLocalBounds();
    if (b.isEmpty() || image.isNull())
        return;

    g.setOpacity (1.0f);
    g.drawImage (resizedImage, b.toFloat(), placement);
}

//==============================================================================
std::unique_ptr<AccessibilityHandler> HighQualityImageComponent::createAccessibilityHandler()
{
    class ImageComponentAccessibilityHandler final : public AccessibilityHandler
    {
    public:
        explicit ImageComponentAccessibilityHandler (HighQualityImageComponent& imageComponentToWrap) :
            AccessibilityHandler (imageComponentToWrap, AccessibilityRole::image),
            imageComponent (imageComponentToWrap)
        {
        }

        String getHelp() const override { return imageComponent.getTooltip(); }

    private:
        HighQualityImageComponent& imageComponent;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageComponentAccessibilityHandler)
    };

    return std::make_unique<ImageComponentAccessibilityHandler> (*this);
}
