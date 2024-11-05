//==============================================================================
#if SQUAREPINE_COMPILE_UNIT_TESTS

class SHA1Tests final : public UnitTest
{
public:
    SHA1Tests() :
        UnitTest ("SHA-1", UnitTestCategories::cryptography)
    {
    }

    void test (const char* input, const String& expected)
    {
        expectEquals (SHA1 (input, strlen (input)).toHexString(), expected);

        {
            const CharPointer_UTF8 utf8 (input);
            expectEquals (SHA1 (utf8).toHexString(), expected);
        }

        {
            MemoryInputStream m (input, strlen (input), false);
            expectEquals (SHA1 (m).toHexString(), expected);
        }
    }

    void runTest() override
    {
        return; // TODO

        beginTest ("Hash Comparisons");

        test ("",                                               "da39a3ee5e6b4b0d3255bfef95601890afd80709");
        test (" ",                                              "b858cb282617fb0956d960215c8e84d1ccf909c6");
        test ("-",                                              "3bc15c8aae3e4124dd409035f32ea2fd6835efc9");
        test ("The quick brown fox jumps over the lazy dog",    "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12");
        test ("The quick brown fox jumps over the lazy dog.",   "408d94384216f890ff7a0c3528e8bed1e0b01621");
    }
};

#endif // SQUAREPINE_COMPILE_UNIT_TESTS
