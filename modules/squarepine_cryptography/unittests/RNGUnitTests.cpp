#if SQUAREPINE_COMPILE_UNIT_TESTS

template<typename Type>
class RNGUnitTestBase : public UnitTest
{
public:
    RNGUnitTestBase (const String& name) :
        UnitTest (name, "RNG")
    {
    }

    //==============================================================================
    virtual Type generateNext() = 0;
    virtual bool isPossiblySecure() const = 0;

    //==============================================================================
    void runTest() override
    {
        beginTest ("Non-zero generation");
        expect (testForZeros(), "The RNG has a lack of an internal seed or was otherwise improperly configured.");

        beginTest ("Collisions");
        const auto threshold = isPossiblySecure()
                                ? maximumSecureRepetitionThreshold
                                : maximumInsecureRepetitionThreshold;

        expect (testForCollisions (threshold), "The RNG has generated the same number more than the desired threshold.");
    }

private:
    //==============================================================================
    enum
    {
        /** The aim is to have a sample size large enough for a good set of comparisons
            while not holding up the overall testing process for too long.
        */
        maxNumCollisionsPerIterationTest = 10,

        /** The aim is to have a sample size large enough for a good set of comparisons
            while not holding up the overall testing process for too long.
        */
        globalRNGSampleSize = 50000,

        /** If surpassed, this would indicate the RNG to be simply too predictable. */
        maximumInsecureRepetitionThreshold = 50,

        /** If surpassed, this would indicate the RNG to be simply too predictable. */
        maximumSecureRepetitionThreshold = 10
    };

    //==============================================================================
    /** Generates a series of random values and tests to see if the generated values are zeros. */
    bool testForZeros()
    {
        constexpr auto zero = static_cast<Type> (0);
        for (int i = 0; i < globalRNGSampleSize; ++i)
            if (generateNext() == zero)
                return false;

        return true;
    }

    /** Generates a series of random values and tests to see if the generated values are duplicated. */
    bool testForCollisionsIteration (int repetitionThreshold)
    {
        Array<Type> generatedValues;
        generatedValues.ensureStorageAllocated (globalRNGSampleSize);

        int numRepetitions = 0;

        for (int i = 0; i < globalRNGSampleSize; ++i)
        {
            const auto generatedValue = generateNext();

            if (std::binary_search (generatedValues.begin(), generatedValues.end(), generatedValue))
            {
                ++numRepetitions;
                if (numRepetitions > repetitionThreshold)
                    return false;
            }

            // Note: Inserting the value in a sorted manner to reduce search time,
            //       and to be compatible with the binary search.
            generatedValues.addUsingDefaultSort (generatedValue);
        }

        return true;
    }

    /** Generates a series of random values and tests to see if the generated values are duplicated. */
    bool testForCollisions (int repetitionThreshold)
    {
        int numFailedCollisionTests = 0;

        for (int i = 0; i < maxNumCollisionsPerIterationTest; ++i)
            if (! testForCollisionsIteration (repetitionThreshold))
                ++numFailedCollisionTests;

        return numFailedCollisionTests < repetitionThreshold;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RNGUnitTestBase)
};

//==============================================================================
class BlumBlumShubUnitTests final : public RNGUnitTestBase<uint64>
{
public:
    BlumBlumShubUnitTests() : RNGUnitTestBase ("BlumBlumShub") { }
    uint64 generateNext() override { return bbs.generate(); }
    bool isPossiblySecure() const override { return false; }

private:
    BlumBlumShub bbs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BlumBlumShubUnitTests)
};

//==============================================================================
class Xorshift32UnitTests final : public RNGUnitTestBase<uint32>
{
public:
    Xorshift32UnitTests() : RNGUnitTestBase ("Xorshift32") { }
    uint32 generateNext() override { return xorshift.generate(); }
    bool isPossiblySecure() const override { return false; }

private:
    Xorshift32 xorshift;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift32UnitTests)
};

class Xorshift64UnitTests final : public RNGUnitTestBase<uint64>
{
public:
    Xorshift64UnitTests (const String& name, Xorshift64::Algorithm algo) :
        RNGUnitTestBase ("Xorshift64 (" + name + ")"),
        xorshift (algo)
    {
    }

    uint64 generateNext() override { return xorshift.generate(); }
    bool isPossiblySecure() const override { return false; }

private:
    Xorshift64 xorshift;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Xorshift64UnitTests)
};

//==============================================================================
class ISAACUnitTests final : public RNGUnitTestBase<uint32>
{
public:
    ISAACUnitTests() : RNGUnitTestBase ("ISAAC") { }
    uint32 generateNext() override { return isaac.generate(); }
    bool isPossiblySecure() const override { return true; }

private:
    ISAAC isaac;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ISAACUnitTests)
};

#endif // SQUAREPINE_COMPILE_UNIT_TESTS
