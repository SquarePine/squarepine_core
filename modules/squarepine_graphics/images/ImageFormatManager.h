/** */
class ImageFormatManager final
{
public:
    /** Constructor. */
    ImageFormatManager() = default;

    //==============================================================================
    /** Adds a format to the manager's list of available file types.

        The object passed-in will be deleted by this object, so don't keep a pointer to it!
    */
    void registerFormat (ImageFileFormat* newFormat);

    /** Handy method to make it easy to register the formats that come with JUCE and this module.

        Currently, this will add PNG, JPEG, GIF and TGA to the list.
    */
    void registerBasicFormats();

    /** Clears the list of known formats. */
    void clearFormats();

    /** @returns the number of currently registered image formats. */
    int getNumKnownFormats() const noexcept                                 { return knownFormats.size(); }

    /** @returns one of the registered file formats. */
    ImageFileFormat* getKnownFormat (int index) const noexcept              { return knownFormats[index]; }

    /** @returns one of the registered file formats. */
    ImageFileFormat* getKnownFormatUnchecked (int index) const noexcept     { return knownFormats.getUnchecked (index); }

    /** Iterator access to the list of known formats. */
    ImageFileFormat* const* begin() const noexcept                          { return knownFormats.begin(); }

    /** Iterator access to the list of known formats. */
    ImageFileFormat* const* end() const noexcept                            { return knownFormats.end(); }

    /** Iterator access to the list of known formats. */
    ImageFileFormat* const* cbegin() const noexcept                         { return knownFormats.begin(); }

    /** Iterator access to the list of known formats. */
    ImageFileFormat* const* cend() const noexcept                           { return knownFormats.end(); }

    //==============================================================================
    /** Looks for a format that can handle the given stream.

        There are currently built-in decoders for PNG, JPEG, GIF, BMP, and TGA.
        The object that is returned should not be deleted by the caller.

        @see canUnderstand, decodeImage, loadFrom
    */
    ImageFileFormat* findFormatForStream (InputStream& input);

    /** Looks for a format that can handle the given file extension.

        There are currently built-in decoders for PNG, JPEG, GIF, BMP, and TGA.
        The object that is returned should not be deleted by the caller.

        @see canUnderstand, decodeImage, loadFrom
    */
    ImageFileFormat* findFormatForFile (const File& file) const;

    //==============================================================================
    /** Tries to load an image from a stream.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns The image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (InputStream& input);

    /** Tries to load an image from a file.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns The image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (const File& file);

    /** Tries to load an image from a block of raw image data.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns The image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (const void* rawData, size_t numBytesOfData);

    //==============================================================================
    /** @returns an Image loaded from Base64 data, or an invalid Image on failure. */
    Image fromBase64 (const String& base64Data);

    /** @returns a base64 string representing the image's data, or an empty string on failure. */
    String toBase64 (const Image&);

private:
    //==============================================================================
    OwnedArray<ImageFileFormat> knownFormats;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageFormatManager)
};
