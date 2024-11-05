OwnedArray<UnitTest> SquarePineCryptographyUnitTestGatherer::createTests()
{
    OwnedArray<UnitTest> tests;

   #if SQUAREPINE_COMPILE_UNIT_TESTS
    tests.add (new CRCTests());
    tests.add (new SHA1Tests());
    tests.add (new BlumBlumShubUnitTests());
    tests.add (new ISAACUnitTests());
    tests.add (new Xorshift32UnitTests());
    tests.add (new Xorshift64UnitTests ("Standard", Xorshift64::Algorithm::standard));
    tests.add (new Xorshift64UnitTests ("Star", Xorshift64::Algorithm::star));
    tests.add (new Xorshift64UnitTests ("xorshift128p", Xorshift64::Algorithm::xorshift128p));
    tests.add (new Xorshift64UnitTests ("xoshiro256pp", Xorshift64::Algorithm::xoshiro256pp));
    tests.add (new Xorshift64UnitTests ("xoshiro256ss", Xorshift64::Algorithm::xoshiro256ss));
    tests.add (new Xorshift64UnitTests ("xoshiro256p", Xorshift64::Algorithm::xoshiro256p));
    tests.add (new Xorshift64UnitTests ("xorshift1024s", Xorshift64::Algorithm::xorshift1024s));
   #endif

    return tests;
}
