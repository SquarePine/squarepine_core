struct SHA1Processor
{
    SHA1Processor()
    {
        // have to copy this data manually, as VS2013 doesn't support member array initialisers
        const uint32 initialState[] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
        std::memcpy (state, initialState, sizeof (state));
    }

    // expects 256 bytes of data
    void processFullBlock (const void* data) noexcept
    {
        uint32 s[5], w[80];
        std::memcpy (s, state, sizeof (s));
        std::memset (w, 0, sizeof (w));

        //Extend the sixteen 32-bit words into eighty 32-bit words:
        auto* source = static_cast<const uint8_t*> (data);
        (void) source;

        for (size_t i = 16; i < 79; ++i)
            w[i] = rotateLeft (w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

        auto a = state[0];
        auto b = state[1];
        auto c = state[2];
        auto d = state[3];
        auto e = state[4];

        for (size_t i = 0; i < 80; ++i)
        {
            uint32 k = 0, f = 0;

            if (isPositiveAndBelow (i, 19))
            {
                f = (b & c) | ((! b) & d);
                k = 0x5a827999;
            }
            else if (i >= 20 && i <= 39)
            {
                f = b ^ c ^ d;
                k = 0x6ed9eba1;
            }
            else if (i >= 40 && i <= 59)
            {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8f1bbcdc;
            }
            else if (i >= 60 && i <= 79)
            {
                f = b ^ c ^ d;
                k = 0xca62c1d6;
            }

            auto temp = rotateLeft (a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rotateLeft (b, 30);
            b = a;
            a = temp;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;

        length += 64;
    }

    void processFinalBlock (const void* data, uint32 numBytes) noexcept
    {
        jassert (numBytes < 64);

        length += numBytes;
        length *= 8; // (the length is stored as a count of bits, not bytes)

        uint8 finalBlocks[128];

        memcpy (finalBlocks, data, numBytes);
        finalBlocks[numBytes++] = 128; // append a '1' bit

        while (numBytes != 56 && numBytes < 64 + 56)
            finalBlocks[numBytes++] = 0; // pad with zeros..

        for (int i = 8; --i >= 0;)
            finalBlocks[numBytes++] = (uint8) (length >> (i * 8)); // append the length.

        jassert (numBytes == 64 || numBytes == 128);

        processFullBlock (finalBlocks);

        if (numBytes > 64)
            processFullBlock (finalBlocks + 64);
    }

    void copyResult (uint8* result) const noexcept
    {
        for (const auto s : state)
        {
            *result++ = (uint8) (s >> 24);
            *result++ = (uint8) (s >> 16);
            *result++ = (uint8) (s >> 8);
            *result++ = (uint8) s;
        }
    }

    void processStream (InputStream& input, int64_t numBytesToRead, uint8* result)
    {
        if (numBytesToRead < 0)
            numBytesToRead = std::numeric_limits<int64_t>::max();

        for (;;)
        {
            uint8 buffer[64];
            auto bytesRead = input.read (buffer, (int) std::min (numBytesToRead, (int64_t) sizeof (buffer)));

            if (bytesRead < (int) sizeof (buffer))
            {
                processFinalBlock (buffer, (unsigned int) bytesRead);
                break;
            }

            numBytesToRead -= (int64_t) sizeof (buffer);
            processFullBlock (buffer);
        }

        copyResult (result);
    }

private:
    uint32 state[5];
    uint64 length = 0;

    static inline uint32 rotateLeft (uint32 x, uint32 y) noexcept { return (x << y) | (x >> (32 - y)); }
};

//==============================================================================
SHA1::SHA1 (const MemoryBlock& data)
{
    process (data.getData(), data.getSize());
}

SHA1::SHA1 (const void* data, size_t numBytes)
{
    process (data, numBytes);
}

SHA1::SHA1 (InputStream& input, int64 numBytesToRead)
{
    SHA1Processor processor;
    processor.processStream (input, numBytesToRead, result);
}

SHA1::SHA1 (const File& file)
{
    FileInputStream fin (file);

    if (fin.getStatus().wasOk())
    {
        SHA1Processor processor;
        processor.processStream (fin, -1, result);
    }
    else
    {
        zerostruct (result);
    }
}

SHA1::SHA1 (CharPointer_UTF8 utf8) noexcept
{
    jassert (utf8.getAddress() != nullptr);
    process (utf8.getAddress(), utf8.sizeInBytes() - 1);
}

void SHA1::process (const void* data, size_t numBytes)
{
    MemoryInputStream m (data, numBytes, false);
    SHA1Processor processor;
    processor.processStream (m, -1, result);
}

MemoryBlock SHA1::getRawData() const
{
    return MemoryBlock (result, sizeof (result));
}

String SHA1::toHexString() const
{
    return String::toHexString (result, sizeof (result), 0);
}

bool SHA1::operator== (const SHA1& other) const noexcept  { return memcmp (result, other.result, sizeof (result)) == 0; }
bool SHA1::operator!= (const SHA1& other) const noexcept  { return ! operator== (other); }

//==============================================================================
#if SQUAREPINE_COMPILE_UNIT_TESTS

class SHA1Tests final : public UnitTest
{
public:
    SHA1Tests() :
        UnitTest ("SHA-1", UnitTestCategories::cryptography)
    {
    }

    void test (const char* input, const char* expected)
    {
        {
            SHA1 hash (input, strlen (input));
            expectEquals (hash.toHexString(), String (expected));
        }

        {
            CharPointer_UTF8 utf8 (input);
            SHA1 hash (utf8);
            expectEquals (hash.toHexString(), String (expected));
        }

        {
            MemoryInputStream m (input, strlen (input), false);
            SHA1 hash (m);
            expectEquals (hash.toHexString(), String (expected));
        }
    }

    void runTest() override
    {
        // beginTest ("SHA1");

        // test ("", "da39a3ee5e6b4b0d3255bfef95601890afd80709");
        // test ("The quick brown fox jumps over the lazy dog",  "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
        // test ("The quick brown fox jumps over the lazy dog.", "408d94384216f890ff7a0c3528e8bed1e0b01621");
    }
};

#endif
