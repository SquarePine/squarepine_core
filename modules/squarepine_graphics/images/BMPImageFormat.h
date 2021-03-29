/** Support for reading and writing Bitmap files.

    Supports uncompressed 8, 24 and 32 bit images. Always writes 32 bit images.
    That should be enough to cover 99.9% of BMP files.

    @warning Does not support 1, 4, 16 bit colour images or images with RLE compression.
*/
class BMPImageFormat final : public ImageFileFormat
{
public:
    /** Constructor. */
    BMPImageFormat() = default;

    //==============================================================================
    /** @internal */
    String getFormatName() override;
    /** @internal */
    bool canUnderstand (InputStream& input) override;
    /** @internal */
    bool usesFileExtension (const File& possibleFile) override;
    /** @internal */
    Image decodeImage (InputStream& input) override;
    /** @internal */
    bool writeImageToStream (const Image& sourceImage, OutputStream& destStream) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BMPImageFormat)
};

