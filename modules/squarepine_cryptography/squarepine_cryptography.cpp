#include "squarepine_cryptography.h"

namespace sp
{
    using namespace juce;

    #include "hash/CityHash.cpp"
    #include "hash/CRC.cpp"
    //#include "hash/SHA1.cpp"
    #include "rng/BlumBlumShub.cpp"
    #include "rng/ISAAC.cpp"
    #include "rng/Xorshift.cpp"
    #include "unittests/CityHashUnitTests.cpp"
    #include "unittests/CRCUnitTests.cpp"
    #include "unittests/RNGUnitTests.cpp"
    //#include "unittests/SHAUnitTests.cpp"
    #include "unittests/SquarePineCryptographyUnitTestGatherer.cpp"
}
