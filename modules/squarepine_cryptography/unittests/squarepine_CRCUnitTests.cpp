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
        runByteTest8();
        runByteTest16();
        runByteTest32();
        runByteTest64();
    }

private:
    template<typename Type>
    void runByteTest (Type polynomial, Type initialValue, Type xorOutValue,
                      bool shouldReflectIn, bool shouldReflectOut,
                      Type expectedValue)
    {
        CRC<Type> crcTest (polynomial, initialValue, xorOutValue, shouldReflectIn, shouldReflectOut);
        const auto resultValue = crcTest.processByte ('a').finalise().get();

        expectEquals ((uint64) expectedValue, (uint64) resultValue,
                      String ("Expected: X, Result: Y")
                        .replace ("X", "0x" + String::toHexString (expectedValue))
                        .replace ("Y", "0x" + String::toHexString (resultValue)));
    }

    void runByteTest8()
    {
        beginTest ("CRC8 - Single Byte Processing");

        runByteTest<uint8> (0x31, 0x00, 0x00, true, true, 0x3b);
        runByteTest<uint8> (0x32, 0x00, 0xff, true, true, 0xad);
        runByteTest<uint8> (0x07, 0xff, 0x00, true, true, 0x16);
        runByteTest<uint8> (0x08, 0xff, 0xff, true, true, 0xf4);

		runByteTest<uint8> (0xd5, 0x00, 0x00, false, false, 0xec);
        runByteTest<uint8> (0x49, 0x00, 0xff, false, false, 0x2a);
        runByteTest<uint8> (0x1d, 0xff, 0x00, false, false, 0x4d);
        runByteTest<uint8> (0x1d, 0xff, 0xff, false, false, 0xb2);
        runByteTest<uint8> (0x1d, 0xfd, 0x00, false, false, 0x77);
        runByteTest<uint8> (0x1d, 0xc7, 0x00, false, false, 0xef);
    }

    void runByteTest16()
    {
        beginTest ("CRC16 - Single Byte Processing");

        runByteTest<uint16> (0x8005, 0x0000, 0x0000, true, true, 0xe8c1);
        runByteTest<uint16> (0x3d65, 0x0000, 0xffff, true, true, 0xb350);
        runByteTest<uint16> (0x1021, 0xffff, 0x0000, true, true, 0x7d08);
        runByteTest<uint16> (0x1021, 0xffff, 0xffff, true, true, 0x82f7);

        runByteTest<uint16> (0x0589, 0x0000, 0x0000, false, false, 0xd360);
        runByteTest<uint16> (0x0589, 0x0000, 0x0001, false, false, 0xd361);
        runByteTest<uint16> (0xc867, 0xffff, 0x0000, false, false, 0x7f83);
        runByteTest<uint16> (0x3d65, 0x0000, 0xffff, false, false, 0xe137);
        runByteTest<uint16> (0x1021, 0xffff, 0xffff, false, false, 0x6288);
    }

    void runByteTest32()
    {
        beginTest ("CRC32 - Single Byte Processing");

        runByteTest<uint32> (0x8001801b, 0x00000000, 0x00000000, true, true, 0xa6916101);
        runByteTest<uint32> (0xf4acfb13, 0x00000000, 0xffffffff, true, true, 0x3592086e);
        runByteTest<uint32> (0x04c11db7, 0xffffffff, 0x00000000, true, true, 0x174841bc);
        runByteTest<uint32> (0x174841bc, 0xffffffff, 0xffffffff, true, true, 0xd651e489);

        runByteTest<uint32> (0x814141ab, 0x00000000, 0x00000000, false, false, 0xd1112b6b);
        runByteTest<uint32> (0x04c11db7, 0x00000000, 0xffffffff, false, false, 0x579b24df);
        runByteTest<uint32> (0x04c11db7, 0xffffffff, 0x00000000, false, false, 0xe66c6494);
        runByteTest<uint32> (0x04c11db7, 0xffffffff, 0xffffffff, false, false, 0x19939b6b);
    }

    void runByteTest64()
    {
        beginTest ("CRC64 - Single Byte Processing");

        runByteTest<uint64> (0x000000000000001B, 0xffffffffffffffff, 0xffffffffffffffff, true, true, 0x3420000000000000);
        runByteTest<uint64> (0x42f0e1eba9ea3693, 0, 0, false, false, 0x548f120162451c62);
    }

    template<typename Type>
    void runStringTests (Type polynomial, Type initialValue, Type xorOutValue,
                         bool shouldReflectIn, bool shouldReflectOut,
                         const String& testStringA, Type expectedValueA,
                         const String& testStringB, Type expectedValueB,
                         const String& testStringC, Type expectedValueC,
                         Type expectedCheckStringValue)
    {
        CRC<Type> crcTest (polynomial, initialValue, xorOutValue, shouldReflectIn, shouldReflectOut);

        struct TestItem final
        {
            String source;
            Type expectedValue = {};
        };

        const std::vector<TestItem> test =
        {
            { testStringA, expectedValueA },
            { testStringB, expectedValueB },
            { testStringC, expectedValueC },
            { crcTest.getCheckString(), expectedCheckStringValue }
        };

        // expectEquals ((Type) crcTest.processByte ('a').finalise().get(), expectedValue);
    }

    void runStringTests8()
    {
        beginTest ("CRC8 - String Processing");
    }
};

#endif // SQUAREPINE_COMPILE_UNIT_TESTS
