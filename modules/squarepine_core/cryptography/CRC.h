/** */
class CRC final
{
public:
    // Calculates the CRC-1 of a given data buffer
    static uint8_t crc1 (const void* data, size_t length)
    {
        if (data == nullptr || length == 0)
            return 0;

        uint8_t crc = 0;
        if (const auto* bytes = static_cast<const uint8_t*> (data))
        {
            for (size_t i = 0; i < length; ++i)
            {
                crc ^= bytes[i];
                crc &= 1; // Keep only the least significant bit
            }
        }

        return crc;
    }

    // Calculates the CRC-7 of a given data buffer
    static uint8_t crc7 (const void* data, size_t length, uint8_t polynomial = 0x09)
    {
        if (data == nullptr || length == 0)
            return 0;

        uint8_t crc = 0;
        if (const auto* bytes = static_cast<const uint8_t*> (data))
        {
            for (size_t i = 0; i < length; ++i)
            {
                crc ^= bytes[i];
                for (size_t j = 0; j < 8; ++j)
                {
                    const auto mask = -(crc & 0x80);
                    crc = (crc << 1) ^ (polynomial & mask);
                }
            }
        }

        return crc;
    }

    /** Calculates the CRC16 of a given data buffer using the specified polynomial. */
    static uint16_t crc16 (uint8_t byte, uint16_t lastCRC16, uint16_t polynomial = 0xa001)
    {
        lastCRC16 ^= byte;
        for (int i { 0 }; i < 8; ++i)
        {
            if (lastCRC16 & 1)
                lastCRC16 = (lastCRC16 >> 1) ^ polynomial;
            else
                lastCRC16 = (lastCRC16 >> 1);
        }

        return lastCRC16;
    }

    /** Calculates the CRC32 of a given data buffer using the specified polynomial. */
    static uint32_t crc32 (uint8_t byte, uint32_t lastCRC32, uint32_t polynomial = 0x04c11db7)
    {
        lastCRC32 ^= byte;

        for (int j = 7; --j >= 0;)
        {
            const auto mask = (uint32_t) -static_cast<int64_t> (lastCRC32 & 1U);
            lastCRC32 = (lastCRC32 >> 1U) ^ (polynomial & mask);
        }

        return lastCRC32;
    }

    // Calculates the CRC-64-ISO of a given data buffer
    static uint64_t calculate_crc64_iso (const void* data, size_t length, uint64_t polynomial = 27)
    {
        if (data == nullptr || length == 0)
            return 0;

        auto crc = UINT64_MAX;
        if (const auto* bytes = static_cast<const uint8_t*> (data))
        {
            for (size_t i = 0; i < length; ++i)
            {
                crc ^= static_cast<uint64_t> (bytes[i]) << 56;
                for (size_t j = 0; j < 8; ++j)
                {
                    const auto mask = -(crc & 0x8000000000000000ULL);
                    crc = (crc << 1) ^ (polynomial & mask);
                }
            }
        }

        return ~crc;
    }
};
