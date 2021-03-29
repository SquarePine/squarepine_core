/** SHA-1 secure hash generator.

    Create one of these objects from a block of source data or a stream, and it
    calculates the SHA-1 hash of that data.

    You can retrieve the hash as a raw 32-byte block, or as a 64-digit hex string.
*/
class SHA1 final
{
public:
    /** Creates an empty SHA1 object.
        The default constructor just creates a hash filled with zeros.
        (This is not equal to the hash of an empty block of data).
    */
    SHA1() = default;

    /** Destructor. */
    ~SHA1() = default;

    /** Creates a copy of another SHA1. */
    SHA1 (const SHA1&) = default;

    /** Copies another SHA1. */
    SHA1& operator= (const SHA1&) = default;

    //==============================================================================
    /** Creates a hash from a block of raw data. */
    explicit SHA1 (const MemoryBlock& data);

    /** Creates a hash from a block of raw data. */
    SHA1 (const void* data, size_t numBytes);

    /** Creates a hash from the contents of a stream.

        This will read from the stream until the stream is exhausted,
        or until maxBytesToRead bytes have been read.
        If maxBytesToRead is negative, the entire stream will be read.
    */
    SHA1 (InputStream& input, int64 maxBytesToRead = -1);

    /** Reads a file and generates the hash of its contents.
        If the file can't be opened, the hash will be left uninitialised (i.e. full of zeros).
    */
    explicit SHA1 (const File& file);

    /** Creates a checksum from a UTF-8 buffer.
        Example:
        @code
            SHA1 checksum (myString.toUTF8());
        @endcode
    */
    explicit SHA1 (CharPointer_UTF8 utf8Text) noexcept;

    //==============================================================================
    /** @returns the hash as a 32-byte block of data. */
    MemoryBlock getRawData() const;

    /** @returns the checksum as a 64-digit hex string. */
    String toHexString() const;

    //==============================================================================
    bool operator== (const SHA1&) const noexcept;
    bool operator!= (const SHA1&) const noexcept;

private:
    //==============================================================================
    uint8 result[16] = {};
    void process (const void*, size_t);

    JUCE_LEAK_DETECTOR (SHA1)
};
