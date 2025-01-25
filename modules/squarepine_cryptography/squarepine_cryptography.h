#ifndef SQUAREPINE_CRYPTOGRAPHY_H
#define SQUAREPINE_CRYPTOGRAPHY_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_cryptography
    vendor:             SquarePine
    version:            1.6.0
    name:               SquarePine Cryptography
    description:        A set of various hash, CRC, and other relevant cryptography functions and classes.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 20
    dependencies:       squarepine_core juce_gui_extra

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_core/squarepine_core.h>

//==============================================================================
#include "rng/Hashing.h"

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "hash/CRC.h"
    //#include "hash/SHA1.h"
    //#include "hash/SHA2.h"
    #include "hash/FNV.h"
    #include "rng/BlumBlumShub.h"
    #include "rng/ISAAC.h"
    #include "rng/Xorshift.h"
    #include "unittests/SquarePineCryptographyUnitTestGatherer.h"
}

#endif // SQUAREPINE_CRYPTOGRAPHY_H
