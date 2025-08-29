template<typename IntegralType>
CRC<IntegralType>::Type CRC<IntegralType>::getCheckValue() const
{
    CRC<Type> test (poly, xorIn, xorOut, reflectIn, reflectOut);
    return test.processString (getCheckString()).finalise().get();
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::reset() noexcept      { crc = xorIn; return *this; }

template<typename IntegralType>
[[nodiscard]] String CRC<IntegralType>::toHexString() const { return "0x" + String::toHexString (crc); }

template<typename IntegralType>
[[nodiscard]] String CRC<IntegralType>::toBinString() const { return "0b" + String (toBitSet().to_string()); }

template<typename IntegralType>
[[nodiscard]] String CRC<IntegralType>::toOctString() const
{
    std::ostringstream str;
    str << std::oct << crc;
    return "0o" + String (str.str());
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::processByte (uint8 data) noexcept
{
    const auto dataConv = reflectIn
                            ? static_cast<Type> (reflect<uint8> (data))
                            : static_cast<Type> (data);

    if constexpr (numBits == 8)
    {
        const auto pos = dataConv ^ crc;
        crc = table[(size_t) pos];
    }
    else if constexpr (numBits == 16)
    {
        const auto pos = (crc >> 8) ^ dataConv;
        crc = (Type) (crc << 8) ^ table[(size_t) pos];
    }
    else
    {
        const auto pos = (crc ^ (dataConv << numBitsToShift)) >> numBitsToShift;
        crc = (Type) (crc << 8) ^ table[(size_t) pos];
    }

    return *this;
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::process (const uint8* data, size_t numBytes) noexcept
{
    jassert (data != nullptr && numBytes > 0);

    if (data != nullptr)
        for (size_t i = 0; i < numBytes; ++i)
            processByte (data[i]);

    return *this;
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::process (const MemoryBlock& data) noexcept
{
    return process (static_cast<const uint8*> (data.getData()), data.getSize());
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::process (const File& file)
{
    if (FileInputStream fis (file); fis.openedOk())
    {
        MemoryBlock data;
        fis.readIntoMemoryBlock (data);
        process (data);
    }

    return *this;
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::processString (const String& data)
{
    return process (reinterpret_cast<const uint8*> (data.toRawUTF8()),
                    data.getNumBytesAsUTF8());
}

template<typename IntegralType>
CRC<IntegralType>& CRC<IntegralType>::finalise() noexcept
{
    if (reflectOut)
        crc = reflect<Type> (crc);

    if (xorOut != zero)
        crc ^= xorOut;

    return *this;
}

template<typename IntegralType>
void CRC<IntegralType>::populateLookupTable()
{
    constexpr auto bitMask = static_cast<Type> (one << (numBits - one));

    // iterate over all possible input byte values 0 - 255
    for (size_t dividend = 0; dividend < 256; ++dividend)
    {
        auto curByte = static_cast<Type> (dividend);
        
        // For 16/32/64-bit CRC, shift the byte into the MSB position
        if constexpr (numBits != 8)
            curByte <<= numBitsToShift;

        for (uint8 bit = 0; bit < 8; ++bit)
        {
            const bool test = (curByte & bitMask) != 0;
            curByte <<= 1;
            if (test)
                curByte ^= poly;
        }

        table[dividend] = (Type) curByte;
    }
}
