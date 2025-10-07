/** An effective analog to AudioFormatManager except for image codecs.

    Simply call registerBasicFormats() in order to be able to read
    all sorts of image formats; some from JUCE itself and some from SquarePine.
*/
class ImageFormatManager final
{
public:
    /** Constructor. */
    ImageFormatManager() = default;

    //==============================================================================
    /** Adds a format to the manager's list of available file types.

        The object passed-in will be owned by this object, so don't keep a pointer to it!
    */
    void registerFormat (std::unique_ptr<ImageFileFormat> newFormat,
                         const String& extensionWildcards);

    /** Handy method to make it easy to register the formats that come with JUCE and this module.

        Currently, this will add PNG, JPEG, GIF, BMP, and TGA to the list.
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
    ImageFileFormat* findFormatForStream (InputStream&);

    /** Looks for a format that can handle the given file extension.

        There are currently built-in decoders for PNG, JPEG, GIF, BMP, and TGA.
        The object that is returned should not be deleted by the caller.

        @see canUnderstand, decodeImage, loadFrom
    */
    ImageFileFormat* findFormatForFile (const File&) const;

    //==============================================================================
    /** @returns a set of wildcards for file-matching that contains
        the extensions for all known formats.

        Example: it might return ".png;.tga" if it just knows about PNGs and Targas.
    */
    String getWildcardForAllFormats (bool sorted = true) const;

    /** Tries to load an image from a stream.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns the image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (InputStream&);

    /** Tries to load an image from a file.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns the image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (const File&);

    /** Tries to load an image from a URL.

        @returns the image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (const URL&);

    /** Tries to load an image from an AndroidDocument.

        @returns the image that was decoded, or an invalid image if it fails.
    */
    Image loadFrom (const AndroidDocument&);

    /** Tries to load an image from a block of raw image data.

        This will use the findImageFormatForStream() method to locate a suitable
        codec, and use that to load the image.

        @returns the image that was decoded, or an invalid image if it fails.
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
    StringArray wildcards;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImageFormatManager)
};
