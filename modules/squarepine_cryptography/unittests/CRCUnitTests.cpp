//==============================================================================
#if SQUAREPINE_COMPILE_UNIT_TESTS

class CRCTests final : public UnitTest
{
public:
    CRCTests() :
        UnitTest ("CRC", UnitTestCategories::cryptography)
    {
    }

    void runTest() override
    {
        test8();
        test16();
        test32();
        test64();
    }

private:
    void test8()
    {
        beginTest ("CRC8");
        {
            CRC<uint8> crcTest (0x31, 0x00, 0x00, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x3B);
        }
        {
            CRC<uint8> crcTest (0x32, 0x00, 0xFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() ==  0xAD);
        }
        {
            CRC<uint8> crcTest (0x07, 0xFF, 0x00, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x16);
        }
        {
            CRC<uint8> crcTest (0x08, 0xFF, 0xFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0xF4);
        }

        {
            CRC<uint8> crcTest (0xD5, 0x00, 0x00, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xEC);
        }
        {
            CRC<uint8> crcTest (0x49, 0x00, 0xFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x2A);
        }
        {
            CRC<uint8> crcTest (0x1D, 0xFF, 0x00, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x4D);
        }
        {
            CRC<uint8> crcTest (0x1D, 0xFF, 0xFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xB2);
        }
        {
            CRC<uint8> crcTest (0x1D, 0xFD, 0x00, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x77);
        }
        {
            CRC<uint8> crcTest (0x1D, 0xC7, 0x00, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xEF);
        }
    }

    void test16()
    {
        beginTest ("CRC16");
        {
            CRC<uint16> crcTest (0x8005, 0x0000, 0x0000, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0xE8C1);
        }
        {
            CRC<uint16> crcTest (0x3D65, 0x0000, 0xFFFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0xB350);
        }
        {
            CRC<uint16> crcTest (0x1021, 0xFFFF, 0x0000, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x7D08);
        }
        {
            CRC<uint16> crcTest (0x1021, 0xFFFF, 0xFFFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x82F7);
        }

        {
            CRC<uint16> crcTest (0x0589, 0x0000, 0x0000, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xD360);
        }
        {
            CRC<uint16> crcTest (0x0589, 0x0000, 0x0001, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xD361);
        }
        {
            CRC<uint16> crcTest (0xC867, 0xFFFF, 0x0000, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x7F83);
        }
        {
            CRC<uint16> crcTest (0x3D65, 0x0000, 0xFFFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xE137);
        }
        {
            CRC<uint16> crcTest (0x1021, 0xFFFF, 0xFFFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x6288);
        }

    }

    void test32()
    {
        beginTest ("CRC32");
        {
            CRC<uint32> crcTest (0x8001801B, 0x00000000, 0x00000000, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0xA6916101);
        }
        {
            CRC<uint32> crcTest (0xF4ACFB13, 0x00000000, 0xFFFFFFFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x3592086E);
        }
        {
            CRC<uint32> crcTest (0x04C11DB7, 0xFFFFFFFF, 0x00000000, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x174841BC);
        }
        {
            CRC<uint32> crcTest (0x174841BC, 0xFFFFFFFF, 0xFFFFFFFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() ==  0xD651E489 );
        }

        {
            CRC<uint32> crcTest (0x814141AB, 0x00000000, 0x00000000, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xD1112B6B);
        }
        {
            CRC<uint32> crcTest (0x04C11DB7, 0x00000000, 0xFFFFFFFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x579B24DF);
        }
        {
            CRC<uint32> crcTest (0x04C11DB7, 0xFFFFFFFF, 0x00000000, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0xE66C6494);
        }
        {
            CRC<uint32> crcTest (0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x19939B6B);
        }
    }

    void test64()
    {
        beginTest ("CRC64");
        {
            CRC<uint64> crcTest (0x000000000000001B, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, true, true);
            jassert (crcTest.processByte ('a').finalise().get() == 0x3420000000000000);
        }

        {
            CRC<uint64> crcTest (0x42F0E1EBA9EA3693, 0x00000000, 0x00000000, false, false);
            jassert (crcTest.processByte ('a').finalise().get() == 0x548F120162451C62);
        }
    }
};

#endif // SQUAREPINE_COMPILE_UNIT_TESTS
