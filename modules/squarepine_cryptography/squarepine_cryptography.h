#ifndef SQUAREPINE_CRYPTOGRAPHY_H
#define SQUAREPINE_CRYPTOGRAPHY_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_cryptography
    vendor:             SquarePine
    version:            1.7.0
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
#include "rng/squarepine_Hashing.h"

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "hash/squarepine_CRC.h"
    //#include "hash/SHA1.h"
    //#include "hash/SHA2.h"
    #include "hash/squarepine_FNV.h"
    #include "rng/squarepine_BlumBlumShub.h"
    #include "rng/squarepine_ISAAC.h"
    #include "rng/squarepine_Xorshift.h"
    #include "unittests/squarepine_SquarePineCryptographyUnitTestGatherer.h"
}

#endif // SQUAREPINE_CRYPTOGRAPHY_H
