
#ifndef SQUAREPINE_SQLITE_H
#define SQUAREPINE_SQLITE_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_sqlite
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine SQLite
    description:        A JUCE module for SQLite.
    website:            https://www.squarepine.io
    license:            Proprietary
    minimumCppStandard: 11
    dependencies:       juce_core

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
#include <juce_core/juce_core.h>

namespace sqlite
{
    extern "C"
    {
        #include "core/sqlite3.h"
    }
}

#endif //SQUAREPINE_SQLITE_H
