#ifndef SQUAREPINE_CRYPTOGRAPHY_H
#define SQUAREPINE_CRYPTOGRAPHY_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_cryptography
    vendor:             SquarePine
    version:            2.0.0
    name:               SquarePine Cryptography
    description:        An assortment of cryptography tools.
    website:            https://www.squarepine.io
    license:            Beerware
    minimumCppStandard: 20
    dependencies:       squarepine_core
    OSXFrameworks:      SystemConfiguration
    iOSFrameworks:      SystemConfiguration

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
#include <squarepine_core/squarepine_core.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    //==============================================================================
    #include "hashing/squarepine_AES.h"
    #include "hashing/squarepine_CRC.h"
    #include "hashing/squarepine_SHA1.h"
    // #include "hashing/squarepine_SHA2.h"
    #include "hashing/squarepine_Skein.h"
    #include "hashing/squarepine_Twofish.h"
    #include "hashing/squarepine_Threefish.h"
    #include "rng/squarepine_BlumBlumShub.h"
    #include "rng/squarepine_ISAAC.h"
    #include "rng/squarepine_Fortuna.h"
    #include "rng/squarepine_Xorshift.h"
    #include "unittests/squarepine_CryptographyUnitTestGatherer.h"
}

//==============================================================================
#include "rng/squarepine_Hashing.h"

#endif // SQUAREPINE_CRYPTOGRAPHY_H
