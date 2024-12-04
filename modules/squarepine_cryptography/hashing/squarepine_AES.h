/** */
class AES final
{
public:
    //==============================================================================
    /** */
    enum class Mode
    {
        ECB,    // Electronic Codebook: not the safest.
        CBC,    // Cipher Block Chaining: the default, and more safe than ECB.
        CFB     // Cipher Feedback: about as safe as CBC.
    };

    //==============================================================================
    /**
    */
    explicit AES (const void* keyData,
                  size_t keyBytes,
                  Mode mode = Mode::CBC);

    //==============================================================================
    /** Encrypts a memory block */
    std::optional<size_t> encrypt (MemoryBlock&) const;

    /** Decrypts a memory block */
    std::optional<size_t> decrypt (MemoryBlock&) const;

    //==============================================================================
    /** Encrypts data in-place

        @param buffer       The message that should be encrypted.
        @param sizeOfMsg    The size of the message that should be encrypted in bytes
        @param bufferSize   The size of the buffer in bytes. To accommodate the encrypted
                            data, the buffer must be larger than the message: the size of
                            the buffer needs to be equal or greater than the size of the
                            message in bytes rounded to the next integer which is divisible
                            by eight. If the message size in bytes is already divisible by eight
                            then you need to add eight bytes to the buffer size. If in doubt
                            simply use bufferSize = sizeOfMsg + 8.

        @returns            The size of the decrypted data in bytes or std::nullopt_t if the encryption failed.
     */
    std::optional<size_t> encrypt (void* buffer, size_t sizeOfMsg, size_t bufferSize) const noexcept;

    /** Decrypts data in-place

        @param buffer  The encrypted data that should be decrypted
        @param bytes   The size of the encrypted data in bytes

        @returns       The size of the decrypted data in bytes or std::nullopt_t if the decryption failed.
    */
    std::optional<size_t> decrypt (void* buffer, size_t bytes) const noexcept;

    //==============================================================================
    /** @returns the standard substitution box (lookup table) used in the Rijndael cipher,
        on which the Advanced Encryption Standard (AES) cryptographic algorithm is based.

        @see https://en.wikipedia.org/wiki/Rijndael_S-box#Example_implementation_in_C_language
    */
    static std::array<uint8, 256> createSBOX();

private:
    //==============================================================================
    static inline constexpr size_t Nb = 4;
    static inline constexpr size_t blockBytesLen = 4 *  Nb * sizeof (uint8);

    using InnerState    = std::array<uint8, Nb>;
    using State         = std::array<InnerState, 4>;

    size_t Nk = 8,
           Nr = 14;

    const MemoryBlock key;
    const Mode mode;
    std::vector<uint8> roundKeys;

    //==============================================================================
    MemoryBlock encryptECB (const MemoryBlock& in);
    MemoryBlock decryptECB (const MemoryBlock& in);
    MemoryBlock encryptCBC (const MemoryBlock& in, const MemoryBlock& iv);
    MemoryBlock decryptCBC (const MemoryBlock& in, const MemoryBlock& iv);
    MemoryBlock encryptCFB (const MemoryBlock& in, const MemoryBlock& iv);
    MemoryBlock decryptCFB (const MemoryBlock& in, const MemoryBlock& iv);

    //==============================================================================
    void expandKey();

    // shift row i on n positions
    void shiftRow (State& state, size_t i, size_t n);
    void shiftRows (State& state);

    void subBytes (State& state);
    void mixColumns (State& state);
    void addRoundKey (State& state, uint8* key);
    void subWord (uint8* a);
    void rotWord (uint8* a);
    void xorWords (uint8* a, uint8* b, uint8* c);
    void rcon (uint8* a, size_t n);
    void invSubBytes (State& state);
    void invMixColumns (State& state);
    void invShiftRows (State& state);
    void encryptBlock (const uint8* in, uint8* out);
    void decryptBlock (const uint8* in, uint8* out);
    void xorBlocks (const uint8* a, const uint8* b, uint8* c, size_t len);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AES)
};
