#if 0

class SHA512 final
{
public:
	SHA512() = default;

	std::string hash (const std::string& input);

private:
	uint64** preprocess(const unsigned char* input, size_t& nBuffer);
	void appendLen(size_t l, uint64& lo, uint64& hi);
	void process(uint64** buffer, size_t nBuffer, uint64* h);
	std::string digest(uint64* h);
	void freeBuffer(uint64** buffer, size_t nBuffer);

	const uint32 SEQUENCE_LEN = 1024 / 64;
	const size_t WORKING_VAR_LEN = 8;
	const size_t MESSAGE_SCHEDULE_LEN = 80;
	const size_t MESSAGE_BLOCK_SIZE = 1024;
};

#endif

/**
    SHA-512 secure hash generator.

    Create one of these objects from a block of source data or a stream,
    and it calculates the SHA-512 hash of that data.

    You can retrieve the hash as a raw 1024-byte block, or as a 128-digit hex string.
    @see MD5, SHA256

    @tags{Cryptography}
*/
class JUCE_API  SHA512
{
public:
    //==============================================================================
    /** Creates an empty SHA512 object.
        The default constructor just creates a hash filled with zeros.
        (This is not equal to the hash of an empty block of data).
    */
    SHA512() noexcept = default;

    /** Destructor. */
    ~SHA512() noexcept = default;

    /** Creates a copy of another SHA512. */
    SHA512 (const SHA512&) = default;

    /** Copies another SHA512. */
    SHA512& operator= (const SHA512&) = default;

    //==============================================================================
    /** Creates a hash from a block of raw data. */
    explicit SHA512 (const MemoryBlock& data);

    /** Creates a hash from a block of raw data. */
    SHA512 (const void* data, size_t numBytes);

    /** Creates a hash from the contents of a stream.

        This will read from the stream until the stream is exhausted, or until
        maxBytesToRead bytes have been read. If maxBytesToRead is negative, the entire
        stream will be read.
    */
    SHA512 (InputStream& input, int64 maxBytesToRead = -1);

    /** Reads a file and generates the hash of its contents.
        If the file can't be opened, the hash will be left uninitialised
        (i.e. full of zeros).
    */
    explicit SHA512 (const File& file);

    /** Creates a checksum from a UTF-8 buffer.
        E.g.
        @code SHA512 checksum (myString.toUTF8());
        @endcode
    */
    explicit SHA512 (CharPointer_UTF8 utf8Text) noexcept;

    //==============================================================================
    /** Returns the hash as a 64-byte block of data. */
    MemoryBlock getRawData() const;

    /** Returns the checksum as a 128-digit hex string. */
    String toHexString() const;

    //==============================================================================
    bool operator== (const SHA512&) const noexcept;
    bool operator!= (const SHA512&) const noexcept;

private:
    //==============================================================================
    uint8 result[64] = {};
    void process (const void*, size_t);

    JUCE_LEAK_DETECTOR (SHA512)
};
