#include "squarepine_cryptography.h"

//==============================================================================
namespace sp
{
    #include "hashing/squarepine_AES.cpp"
    #include "hashing/squarepine_CRC.cpp"
    #include "hashing/squarepine_SHA1.cpp"
    #include "hashing/squarepine_Skein.cpp"
    #include "hashing/squarepine_Twofish.cpp"
    #include "hashing/squarepine_Threefish.cpp"
    #include "rng/squarepine_BlumBlumShub.cpp"
    #include "rng/squarepine_Fortuna.cpp"
    #include "rng/squarepine_ISAAC.cpp"
    #include "rng/squarepine_Xorshift.cpp"
    #include "unittests/squarepine_CRCUnitTests.cpp"
    #include "unittests/squarepine_RNGUnitTests.cpp"
    #include "unittests/squarepine_SHAUnitTests.cpp"
    #include "unittests/squarepine_CryptographyUnitTestGatherer.cpp"
}
