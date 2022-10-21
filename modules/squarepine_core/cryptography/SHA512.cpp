namespace sha512
{
	constexpr uint64 hPrime[8] =
    {
        0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
    };

	constexpr uint64 k[80] =
    {
        0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
        0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
        0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
        0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
        0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
        0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
        0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
        0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
        0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
        0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
        0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
        0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
        0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
        0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
        0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
        0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
    };

	#define Ch(x,y,z)   ((x & y) ^ (~x & z))
	#define Maj(x,y,z)  ((x & y) ^ (x & z) ^ (y & z))
	#define RotR(x, n)  ((x >> n) | (x << ((sizeof(x) << 3) - n)))
	#define Sig0(x)     ((RotR(x, 28)) ^ (RotR(x,34)) ^ (RotR(x, 39)))
	#define Sig1(x)     ((RotR(x, 14)) ^ (RotR(x,18)) ^ (RotR(x, 41)))
	#define sig0(x)     (RotR(x, 1) ^ RotR(x,8) ^ (x >> 7))
	#define sig1(x)     (RotR(x, 19) ^ RotR(x,61) ^ (x >> 6))
}

#if 0
std::string SHA512::hash(const std::string& input)
{
	size_t nBuffer = {}; // amount of message blocks
	auto* h = new uint64[8]; // buffer holding the message digest (512-bit = 8 64-bit words)

	// message block buffers (each 1024-bit = 16 64-bit words)
	auto buffer = preprocess((unsigned char*) input.c_str(), nBuffer);
	process(buffer, nBuffer, h);

	freeBuffer(buffer, nBuffer);
	return digest(h);
}

uint64** SHA512::preprocess(const unsigned char* input, size_t &nBuffer)
{
	// Padding: input || 1 || 0*k || l (in 128-bit representation)
	auto mLen = strlen((const char*) input);
	auto l = mLen * CHAR_BIT; // length of input in bits
	auto k = (896-1-l) % MESSAGE_BLOCK_SIZE; // length of zero bit padding (l + 1 + k = 896 mod 1024)
	nBuffer = (l+1+k+128) / MESSAGE_BLOCK_SIZE;

	auto** buffer = new uint64*[nBuffer];

	for (size_t i = 0; i < nBuffer; ++i)
		buffer[i] = new uint64[SEQUENCE_LEN];

	// Either copy existing message, add 1 bit or add 0 bit
	for (size_t i = 0; i < nBuffer; ++i)
    {
		for (size_t j = 0; j < SEQUENCE_LEN; ++j)
        {
			auto in = 0x0ULL;
			for (size_t k = 0; k < CHAR_BIT; k++)
            {
				auto index = i * 128 + j * 8 + k;

				if (index < mLen)       in = in << 8 | (uint64)input[index];
				else if (index == mLen) in = in << 8 | 0x80ULL;
				else                    in = in << 8 | 0x0ULL;
			}

			buffer[i][j] = in;
		}
	}

	// Append the length to the last two 64-bit blocks
	appendLen(l, buffer[nBuffer-1][SEQUENCE_LEN-1], buffer[nBuffer-1][SEQUENCE_LEN-2]);
	return buffer;
}

void SHA512::process(uint64** buffer, size_t nBuffer, uint64* h)
{
	uint64 s[WORKING_VAR_LEN];
	uint64 w[MESSAGE_SCHEDULE_LEN]; 

	memcpy(h, sha512::hPrime, WORKING_VAR_LEN*sizeof(uint64));

	for (size_t i = 0; i < nBuffer; ++i)
    {
		// copy over to message schedule
		memcpy(w, buffer[i], SEQUENCE_LEN*sizeof(uint64));

		// Prepare the message schedule
		for (size_t j=16; j < MESSAGE_SCHEDULE_LEN; ++j)
			w[j] = w[j-16] + sig0(w[j-15]) + w[j-7] + sig1(w[j-2]);

		// Initialize the working variables
		memcpy(s, h, WORKING_VAR_LEN*sizeof(uint64));

		// Compression
		for (size_t j=0; j < MESSAGE_SCHEDULE_LEN; ++j)
        {
			auto temp1 = s[7] + Sig1(s[4]) + Ch(s[4], s[5], s[6]) + sha512::k[j] + w[j];
			auto temp2 = Sig0(s[0]) + Maj(s[0], s[1], s[2]);

			s[7] = s[6];
			s[6] = s[5];
			s[5] = s[4];
			s[4] = s[3] + temp1;
			s[3] = s[2];
			s[2] = s[1];
			s[1] = s[0];
			s[0] = temp1 + temp2;
		}

		// Compute the intermediate hash values
		for(size_t j=0; j < WORKING_VAR_LEN; ++j)
			h[j] += s[j];
	}

}

void SHA512::appendLen(size_t l, uint64& lo, uint64& hi)
{
	lo = l;
	hi = {};
}

std::string SHA512::digest(uint64* h)
{
	std::stringstream ss;
	for (size_t i = 0; i < CHAR_BIT; ++i)
		ss << std::hex << std::setw(16) << std::setfill('0') << h[i];

	delete[] h;
	return ss.str();
}

void SHA512::freeBuffer(uint64** buffer, size_t nBuffer)
{
	for (size_t i = 0; i < nBuffer; ++i)
		delete[] buffer[i];

	delete[] buffer;
}

#endif

struct SHA512Processor final
{
    // expects 128 bytes of data
    void processFullBlock (const void* data) noexcept
    {
        const uint64_t constants[] =
        {
            0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
            0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
        };


        size_t nBuffer = {}; // amount of message blocks
        auto* h = new uint64[8]; // buffer holding the message digest (512-bit = 8 64-bit words)

        // message block buffers (each 1024-bit = 16 64-bit words)
        auto buffer = preprocess((unsigned char*) input.c_str(), nBuffer);
        process(buffer, nBuffer, h);

        freeBuffer(buffer, nBuffer);
        return digest(h);
    }

private:
    uint64_t state[8] =
    {
        0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL, 0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
    };

    uint64_t length = 0;

    constexpr uint64_t rotate (uint64_t x, uint64_t n) noexcept             { return (x >> n) | (x << ((sizeof (x) << 3) - n)); }
    constexpr uint64_t ch (uint64_t x, uint64_t y, uint64_t z) noexcept     { return (x & y) ^ (~x & z); }
    constexpr uint64_t maj (uint64_t x, uint64_t y, uint64_t z) noexcept    { return (x & y) ^ (x & z) ^ (y & z); }
    constexpr uint64_t s0 (uint64_t x) noexcept                             { return rotate (x, 1) ^ rotate (x, 8) ^ (x >> 7); }
    constexpr uint64_t s1 (uint64_t x) noexcept                             { return rotate (x, 19) ^ rotate (x, 61) ^ (x >> 6); }
    constexpr uint64_t S0 (uint64_t x) noexcept                             { return rotate (x, 28) ^ rotate (x, 34) ^ rotate (x, 39); }
    constexpr uint64_t S1 (uint64_t x) noexcept                             { return rotate (x, 14) ^ rotate (x, 18) ^ rotate (x, 41); }
};

//==============================================================================
SHA512::SHA512 (const void* data, size_t numBytes)
{
    process (data, numBytes);
}

SHA512::SHA512 (const MemoryBlock& data) :
    SHA512 (data.getData(), data.getSize())
{
}

SHA512::SHA512 (InputStream& input, int64 numBytesToRead)
{
    SHA512Processor ().processStream (input, numBytesToRead, result);
}

SHA512::SHA512 (const File& file)
{
    FileInputStream fin (file);

    if (fin.getStatus().wasOk())
        SHA512Processor ().processStream (fin, -1, result);
}

SHA512::SHA512 (CharPointer_UTF8 utf8) noexcept
{
    jassert (utf8.getAddress() != nullptr);
    process (utf8.getAddress(), utf8.sizeInBytes() - 1);
}

void SHA512::process (const void* data, size_t numBytes)
{
    jassert (data != nullptr);

    MemoryInputStream m (data, numBytes, false);
    SHA512Processor ().processStream (m, -1, result);
}

MemoryBlock SHA512::getRawData() const
{
    return { result, sizeof (result) };
}

String SHA512::toHexString() const
{
    return String::toHexString (result, sizeof (result), 0);
}

bool SHA512::operator== (const SHA512& other) const noexcept  { return std::memcmp (result, other.result, sizeof (result)) == 0; }
bool SHA512::operator!= (const SHA512& other) const noexcept  { return ! operator== (other); }


//==============================================================================
#if 1 // JUCE_UNIT_TESTS

class SHA512Tests final : public UnitTest
{
public:
    SHA512Tests()
        : UnitTest ("SHA-512", UnitTestCategories::cryptography)
    {}

    void test (const char* input, const char* expected)
    {
        CharPointer_UTF8 utf8 (input);
        const auto numBytes = utf8.sizeInBytes ();

        expectEquals (SHA512 (input, numBytes).toHexString(), expected);
        expectEquals (SHA512 (utf8).toHexString(), expected);
        MemoryInputStream m (input, numBytes, false);
        expectEquals (SHA512 (m).toHexString(), expected);
    }

    void runTest() override
    {
        beginTest ("SHA512");

        test ("", "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
        test ("a", "1f40fc92da241694750979ee6cf582f2d5d7d28e18335de05abc54d0560e0f5302860c652bf08d560252aa5e74210546f369fbbbce8c12cfc7957b2652fe9a75");
        test ("abc", "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
        test ("message digest", "107dbf389d9e9f71a3a95f6c055b9251bc5268c2be16d6c13492ea45b0199f3309e16455ab1e96118e8a905d5597b72038ddb372a89826046de66687bb420e7c");
        test ("abcdefghijklmnopqrstuvwxyz", "4dbff86cc2ca1bae1e16468a05cb9881c97f1753bce3619034898faa1aabe429955a1bf8ec483d7421fe3c1646613a59ed5441fb0f321389f77f48a879c7b1f1");
        test ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", "1e07be23c26a86ea37ea810c8ec7809352515a970e9253c26f536cfc7a9996c45c8370583e0a78fa4a90041d71a4ceab7423f19c71b9d5a3e01249f0bebd5894");
        test ("The quick brown fox jumps over the lazy dog",  "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6");
        test ("The quick brown fox jumps over the lazy dog.", "91ea1245f20d46ae9a037a989f54f1f790f0a47607eeb8a14d12890cea77a1bbc6c7ed9cf205e67b7f2b8fd4c7dfd3a7a8617e45f3c463d481c7e586c39ac1ed");
    }
};

static SHA512Tests SHA512UnitTests;

#endif
