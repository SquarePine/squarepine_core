//==============================================================================
struct SHA1Processor final
{
    using ResultArray = std::array<uint8, 20>;
    using State = std::array<uint32, 5>;

    SHA1Processor() noexcept = default;

    ResultArray processStream (InputStream& input, int64 numBytesToRead)
    {
        if (numBytesToRead < 0)
            numBytesToRead = std::numeric_limits<int64>::max();

        ResultArray buffer;
        for (;;)
        {
            buffer.fill (0);
            auto bytesRead = input.read (buffer.data(), (int) jmin (numBytesToRead, (int64) buffer.size()));

            if (bytesRead < (int) buffer.size())
            {
                processFinalBlock (buffer.data(), (uint32) bytesRead);
                break;
            }

            numBytesToRead -= (int64) buffer.size();
            processFullBlock (buffer.data(), buffer.size());
        }

        return copyResult();
    }

private:
    State state = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };
    uint64 length = 0;

    static constexpr uint32 ch (uint32 x, uint32 y, uint32 z) noexcept  { return z ^ ((y ^ z) & x); }
    static constexpr uint32 maj (uint32 x, uint32 y, uint32 z) noexcept { return y ^ ((y ^ z) & (x ^ y)); }
    static constexpr uint32 s0 (uint32 x) noexcept                      { return std::rotr (x, 7)  ^ std::rotr (x, 18) ^ (x >> 3); }
    static constexpr uint32 s1 (uint32 x) noexcept                      { return std::rotr (x, 17) ^ std::rotr (x, 19) ^ (x >> 10); }
    static constexpr uint32 S0 (uint32 x) noexcept                      { return std::rotr (x, 2)  ^ std::rotr (x, 13) ^ std::rotr (x, 22); }
    static constexpr uint32 S1 (uint32 x) noexcept                      { return std::rotr (x, 6)  ^ std::rotr (x, 11) ^ std::rotr (x, 25); }

    ResultArray copyResult() const noexcept
    {
        ResultArray result;
        auto* rawData = result.data();

        for (auto s : state)
        {
            *rawData++ = (uint8) (s >> 24);
            *rawData++ = (uint8) (s >> 16);
            *rawData++ = (uint8) (s >> 8);
            *rawData++ = (uint8) s;
        }

        return result;
    }

    // expects 20 bytes of data
    void processFullBlock (const void* data, uint64 numBytes) noexcept
    {
        uint32 W[80]        = { 0 };
        uint8 datatail[128] = { 0 };
        uint64 databits     = numBytes * 8;
        uint32 loopcount    = (numBytes + 8) / 64 + 1;
        uint32 tailbytes    = 64 * loopcount - numBytes;

        uint32 a = 0,
               b = 0,
               c = 0,
               d = 0,
               e = 0,
               k = 0,
               temp = 0;

        /** Pre-processing of data tail (includes padding to fill out 512-bit chunk):
            Add bit '1' to end of message (big-endian)
            Add 64-bit message length in bits at very end (big-endian)
        */
        datatail[0]             = 0x80;
        datatail[tailbytes - 8] = (uint8) (databits >> 56 & 0xff);
        datatail[tailbytes - 7] = (uint8) (databits >> 48 & 0xff);
        datatail[tailbytes - 6] = (uint8) (databits >> 40 & 0xff);
        datatail[tailbytes - 5] = (uint8) (databits >> 32 & 0xff);
        datatail[tailbytes - 4] = (uint8) (databits >> 24 & 0xff);
        datatail[tailbytes - 3] = (uint8) (databits >> 16 & 0xff);
        datatail[tailbytes - 2] = (uint8) (databits >>  8 & 0xff);
        datatail[tailbytes - 1] = (uint8) (databits >>  0 & 0xff);

        // Process each 512-bit chunk
        for (uint32 lidx = 0; lidx < loopcount; lidx++)
        {
            // Compute all elements in W
            memset (W, 0, 80 * sizeof (uint32));

            // Break 512-bit chunk into sixteen 32-bit, big endian words
            for (uint32 widx = 0; widx <= 15; widx++)
            {
                auto wcount = 24;
                auto didx = 0;

                // Copy byte-per byte from specified buffer
                while (didx < numBytes && wcount >= 0)
                {
                    W[widx] += ((((uint32*) data)[didx]) << wcount);
                    ++didx;
                    wcount -= 8;
                }

                // Fill out W with padding as needed
                while (wcount >= 0)
                {
                    W[widx] += (((uint32) datatail[didx - numBytes]) << wcount);
                    ++didx;
                    wcount -= 8;
                }
            }

            /** Extend the sixteen 32-bit words into eighty 32-bit words, with potential optimization from:
                "Improving the Performance of the Secure Hash Algorithm (SHA-1)" by Max Locktyukhin
            */
            for (uint32 widx = 16; widx <= 31; widx++)
                W[widx] = std::rotl ((W[widx - 3] ^ W[widx - 8] ^ W[widx - 14] ^ W[widx - 16]), 1);

            for (uint32 widx = 32; widx <= 79; widx++)
                W[widx] = std::rotl ((W[widx - 6] ^ W[widx - 16] ^ W[widx - 28] ^ W[widx - 32]), 2);

            // Main loop
            a = state[0];
            b = state[1];
            c = state[2];
            d = state[3];
            e = state[4];

            for (uint32 idx = 0; idx <= 79; idx++)
            {
                uint32 f = 0;

                if (idx <= 19)
                {
                    f = (b & c) | ((~b) & d);
                    k = 0x5a827999;
                }
                else if (idx >= 20 && idx <= 39)
                {
                    f = b ^ c ^ d;
                    k = 0x6ed9eba1;
                }
                else if (idx >= 40 && idx <= 59)
                {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8f1bbcdc;
                }
                else if (idx >= 60 && idx <= 79)
                {
                    f = b ^ c ^ d;
                    k = 0xca62c1d6;
                }

                temp = std::rotl (a, 5) + f + e + k + W[idx];
                e = d;
                d = c;
                c = std::rotl (b, 30);
                b = a;
                a = temp;
            }

            state[0] += a;
            state[1] += b;
            state[2] += c;
            state[3] += d;
            state[4] += e;
        }

#if 0
        /* Store binary digest in supplied buffer */
        if (data)
        {
            for (idx = 0; idx < 5; idx++)
            {
                digest[idx * 4 + 0] = (uint8) (state[idx] >> 24);
                digest[idx * 4 + 1] = (uint8) (state[idx] >> 16);
                digest[idx * 4 + 2] = (uint8) (state[idx] >> 8);
                digest[idx * 4 + 3] = (uint8) (state[idx]);
            }
        }
#endif
    }

    void processFinalBlock (const void* data, uint32 numBytes) noexcept
    {
        jassert (numBytes < 20);

        length += numBytes;
        length *= 8; // (the length is stored as a count of bits, not bytes)

        uint8 finalBlocks[20];

        memcpy (finalBlocks, data, numBytes);
        finalBlocks[numBytes++] = 20; // append a '1' bit

        while (numBytes != 56 && numBytes < 64 + 56)
            finalBlocks[numBytes++] = 0; // pad with zeros..

        for (int i = 8; --i >= 0;)
            finalBlocks[numBytes++] = (uint8) (length >> (i * 8)); // append the length.

        jassert (numBytes == 20);

        processFullBlock (finalBlocks, 20);

        if (numBytes > 20)
            processFullBlock (finalBlocks + 20, numBytes - 20);
    }
};

//==============================================================================
SHA1::SHA1()                                            { result.fill (0); }
SHA1::SHA1 (const MemoryBlock& data)                    { process (data.getData(), data.getSize()); }
SHA1::SHA1 (const void* data, size_t numBytes)          { process (data, numBytes); }
SHA1::SHA1 (InputStream& input, int64 numBytesToRead)   { result = SHA1Processor().processStream (input, numBytesToRead); }
SHA1::SHA1 (CharPointer_UTF8 utf8) noexcept             { process (utf8.getAddress(), utf8.sizeInBytes() - 1); }
MemoryBlock SHA1::getRawData() const                    { return MemoryBlock (result.data(), result.size()); }
String SHA1::toHexString() const                        { return String::toHexString (result.data(), (int) result.size(), 0); }

SHA1::SHA1 (const File& file)
{
    FileInputStream fin (file);

    if (fin.getStatus().wasOk())
        result = SHA1Processor().processStream (fin, -1);
    else
        zerostruct (result);
}

void SHA1::process (const void* data, size_t numBytes)
{
    jassert (data != nullptr && numBytes > 0);

    MemoryInputStream m (data, numBytes, false);
    result = SHA1Processor().processStream (m, -1);
}
