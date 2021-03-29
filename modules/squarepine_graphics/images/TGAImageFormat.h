/** A subclass of ImageFileFormat for reading and writing Targa image files.

    @see ImageFileFormat
*/
class TGAImageFormat final : public ImageFileFormat
{
public:
    /** Constructor. */
    TGAImageFormat() = default;

    //==============================================================================
    /** @internal */
    String getFormatName() override { return "TGA"; }
    /** @internal */
    bool usesFileExtension (const File& possibleFile) override { return possibleFile.hasFileExtension ("tga"); }
    /** @internal */
    bool canUnderstand (InputStream& stream) override;
    /** @internal */
    Image decodeImage (InputStream& stream) override;
    /** @internal */
    bool writeImageToStream (const Image& image, OutputStream& stream) override;

private:
    //==============================================================================
    /** A pixel of type big-endian ARGB (ie: little-endian BGRA, as the members are listed.) */
    struct TargaPixel final
    {
        TargaPixel() noexcept = default;
        TargaPixel (const TargaPixel&) noexcept = default;
        TargaPixel (uint8 bgr, uint8 a = 0) noexcept;
        TargaPixel (uint8 b, uint8 g, uint8 r, uint8 a = 0) noexcept;
        ~TargaPixel() noexcept = default;
        TargaPixel& operator= (const TargaPixel&) noexcept = default;

        uint8 b = 0, g = 0, r = 0, a = 0;
    };

    /** A list of possible Targa image sub-types */
    enum TargaType
    {
        none = 0,
        uncompressedColourMapped,
        uncompressedRGB,
        uncompressedBlackWhite,
        rleColourMapped = 9,
        rleRGB,
        rleBlackWhite,
        compressedColourMapped = 32,
        compressedColourMapped4Pass = 33
    };

    enum RLEPacketType
    {
        raw = 0,
        runLength
    };

    /** The header structure of a Targa image */
    struct TargaHeader final
    {
        int8 idLength = 0;
        int8 colourMapType = 0;
        int8 dataTypeCode = (char) uncompressedRGB; //Image Type
        int16 colourMapOrigin = 0;                  //Colour Map Index
        int16 colourMapLength = 0;
        int8 colourMapDepth = 0;                    //Colour Map Entry Size
        int16 originX = 0, originY = 0, width = 0, height = 0;
        int8 bitsPerPixel = 0;                      //Pixel Depth
        int8 imageDescriptor = 0;
    };

    TargaHeader header;

    //==============================================================================
    class Helpers;
    friend class Helpers;

    class TargaReader;
    friend class TargaReader;

    //==============================================================================
    /** Read the Targa image's header from the provided stream */
    static void readHeader (TargaHeader& result, InputStream& stream);

    /** Write the Targa image's header to the provided stream */
    static void writeHeader (OutputStream& stream, TargaHeader& source);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TGAImageFormat)
};

