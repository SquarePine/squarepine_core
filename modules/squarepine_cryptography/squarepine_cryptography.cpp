#include "squarepine_cryptography.h"

namespace sp
{
    using namespace juce;

    #include "hash/squarepine_CRC.cpp"
    //#include "hash/squarepine_SHA1.cpp"
    #include "rng/squarepine_BlumBlumShub.cpp"
    #include "rng/squarepine_ISAAC.cpp"
    #include "rng/squarepine_Xorshift.cpp"
    #include "unittests/squarepine_CRCUnitTests.cpp"
    #include "unittests/squarepine_RNGUnitTests.cpp"
    //#include "unittests/squarepine_SHAUnitTests.cpp"
    #include "unittests/squarepine_SquarePineCryptographyUnitTestGatherer.cpp"
}
