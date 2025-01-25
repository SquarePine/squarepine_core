/** FNVHash class representing the FNV-0, FNV-1, and FNV-1a hashes.
    Use one of these for setting up hashing of unsigned 32-bit and 64-bit values.

    Create one of these with a block of source data or a stream,
    and it calculates the FNV hash of that data.

    @see https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
    @see http://www.isthe.com/chongo/tech/comp/fnv/index.html
*/
template<typename Type, Type primeValueSource, Type offsetBasisSource, bool use1AAlgorithm>
class FNVHash final
{
public:
    //==============================================================================
    /** Some predetermined prime value as per FNV's specs. */
    static inline constexpr Type primeValue  = primeValueSource;
    /** In FNV-0, this should be 0. Otherwise, this will be some pre-calculated value. */
    static inline constexpr Type offsetBasis = offsetBasisSource;

    //==============================================================================
    /** Creates a null FNVHash object. */
    FNVHash() noexcept = default;

    /** Creates a copy of another FNVHash. */
    FNVHash (const FNVHash&) noexcept = default;

    /** */
    explicit FNVHash (Type initialState) noexcept :
        state (initialState)
    {
    }

    /** Creates a hash for the input from a stream.

        This will read up to the given number of bytes
        from the stream and produce the hash of that.

        If the number of bytes to read is negative,
        it'll read until the stream is exhausted.
    */
    FNVHash (InputStream& input, int64 numBytesToRead = -1)
    {
        if (numBytesToRead < 0)
            numBytesToRead = std::numeric_limits<int64>::max();

        std::array<uint8, 512> tempBuffer;

        while (numBytesToRead > 0 && ! input.isExhausted())
        {
            tempBuffer.fill (0);
            const auto bytesRead = input.read (tempBuffer.data(), (int) jmin (numBytesToRead, (int64) tempBuffer.size()));

            if (bytesRead <= 0)
                break;

            numBytesToRead -= bytesRead;

            for (auto v : tempBuffer)
                process (static_cast<Type> (v)); // Deliberately widened to follow suit with the source C examples.
        }
    }

    /** Creates a hash for a block of binary data. */
    explicit FNVHash (const MemoryBlock& data) noexcept :
        FNVHash (MemoryInputStream (data, false))
    {
    }

    /** Creates a hash for a block of binary data. */
    FNVHash (const void* data, size_t numBytes) noexcept :
        FNVHash (MemoryBlock (data, numBytes))
    {
    }

    /** Creates a hash for the contents of a file. */
    explicit FNVHash (const File& file)
    {
        if (FileInputStream fin (file); fin.openedOk())
            state = FNVHash (fin).get();
    }

    /** Creates a hash of the characters in a UTF-8 buffer.

        Example:
        @code
            FNVHash myResults (myString.toUTF8());
        @endcode
    */
    explicit FNVHash (CharPointer_UTF8 utf8) noexcept :
        FNVHash (utf8.getAddress(), utf8.getAddress() != nullptr ? utf8.sizeInBytes() - 1 : 0)
    {
    }

    /** Destructor. */
    ~FNVHash() noexcept = default;

    //==============================================================================
    /** Copies another FNVHash. */
    FNVHash& operator= (const FNVHash&) noexcept = default;

    //==============================================================================
    /** */
    Type process (Type value) noexcept
    {
        if constexpr (use1AAlgorithm)
        {
            // FNV-1a:
            state ^= value;
            state *= primeValue;
        }
        else
        {
            // FNV-0 and FNV-1:
            state *= primeValue;
            state ^= value;
        }

        return state;
    }

    /** @returns */
    constexpr Type get() const noexcept { return state; }

    /** @returns */
    String toHexString() const          { return String::toHexString (state); }

    //==============================================================================
    /** @returns */
    bool operator== (const FNVHash& other) const noexcept   { return state == other.state; }
    /** @returns */
    bool operator!= (const FNVHash& other) const noexcept   { return ! operator== (other); }

private:
    //==============================================================================
    Type state = offsetBasis;

    //==============================================================================
    JUCE_LEAK_DETECTOR (FNVHash)
};

//==============================================================================
/** An FNV0 implementation for 32-bit values. */
using FNV0Hash32    = FNVHash<uint32, 0x811c9dc5, 0, false>;
/** An FNV1 implementation for 32-bit values. */
using FNV1Hash32    = FNVHash<uint32, 0x811c9dc5, 0x811c9dc5, false>;
/** An FNV1a implementation for 32-bit values. */
using FNV1aHash32   = FNVHash<uint32, 0x811c9dc5, 0x811c9dc5, true>;

/** An FNV0 implementation for 64-bit values. */
using FNV0Hash64    = FNVHash<uint64, 0x00000100000001b3ULL, 0ULL, false>;
/** An FNV1 implementation for 64-bit values. */
using FNV1Hash64    = FNVHash<uint64, 0x00000100000001b3ULL, 0xcbf29ce484222325ULL, false>;
/** An FNV1a implementation for 64-bit values. */
using FNV1aHash64   = FNVHash<uint64, 0x00000100000001b3ULL, 0xcbf29ce484222325ULL, true>;
