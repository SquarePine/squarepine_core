#include "squarepine_cryptography.h"

//==============================================================================
namespace sp
{
    #include "hashing/CRC.cpp"
    #include "hashing/SHA1.cpp"
    #include "rng/BlumBlumShub.cpp"
    #include "rng/ISAAC.cpp"
    #include "rng/Xorshift.cpp"
    #include "unittests/CRCUnitTests.cpp"
    #include "unittests/RNGUnitTests.cpp"
    #include "unittests/SHAUnitTests.cpp"
    #include "unittests/SquarePineCryptographyUnitTestGatherer.cpp"
}
