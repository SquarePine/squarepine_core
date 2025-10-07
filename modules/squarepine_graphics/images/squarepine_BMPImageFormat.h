/** Support for reading and writing Bitmap files.

    Supports uncompressed 8, 24 and 32 bit images. Always writes 32 bit images.
    That should be enough to cover 99.9% of BMP files.
*/
class BMPImageFormat final : public ImageFileFormat
{
public:
    /** Constructor. */
    BMPImageFormat() = default;

    //==============================================================================
    /** @internal */
    String getFormatName() override { return "BMP"; }
    /** @internal */
    bool canUnderstand (InputStream&) override;
    /** @internal */
    bool usesFileExtension (const File&) override;
    /** @internal */
    Image decodeImage (InputStream&) override;
    /** @internal */
    bool writeImageToStream (const Image&, OutputStream&) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BMPImageFormat)
};
