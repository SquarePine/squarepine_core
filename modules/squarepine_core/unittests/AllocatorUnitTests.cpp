#if SQUAREPINE_COMPILE_UNIT_TESTS

class AllocatorTests final : public UnitTest
{
public:
    AllocatorTests() :
        UnitTest ("Allocator", UnitTestCategories::containers)
    {
    }

    void runTest() override
    {
        runTestsWithLock<DummyCriticalSection> ("DummyCriticalSection");
        runTestsWithLock<CriticalSection> ("CriticalSection");
        runTestsWithLock<SpinLock> ("SpinLock");
    }

private:
    template<typename TypeOfLockToUse>
    void runTestsWithLock (StringRef lockTypeName)
    {
        beginTest (String ("Construction - ") + lockTypeName);

        Allocator<TypeOfLockToUse> allocator;

        beginTest ("Primitive Types");

        allocator.allocateObject (int8_t());
        allocator.allocateObject (int16_t());
        allocator.allocateObject (int32_t());
        allocator.allocateObject (int64_t());
        allocator.allocateObject (uint8_t());
        allocator.allocateObject (uint16_t());
        allocator.allocateObject (uint32_t());
        allocator.allocateObject (uint64_t());
        allocator.allocateObject (bool());
        allocator.allocateObject (float());
        allocator.allocateObject (double());

        beginTest ("Complex Types - Default Constructible");

        allocator.template allocateObject<AllocatorTests>();
    }
};

#endif
