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
    #include "hashing/CRC.h"
    #include "hashing/SHA1.h"
    // #include "hashing/SHA2.h"
    #include "rng/BlumBlumShub.h"
    #include "rng/ISAAC.h"
    #include "rng/Xorshift.h"
    #include "unittests/SquarePineCryptographyUnitTestGatherer.h"
}

//==============================================================================
#include "rng/Hashing.h"

#endif // SQUAREPINE_CRYPTOGRAPHY_H
