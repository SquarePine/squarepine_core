#ifndef SQUAREPINE_SQLITE_H
#define SQUAREPINE_SQLITE_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_sqlite
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine SQLite
    description:        A JUCE module for SQLite.
    website:            https://www.squarepine.io
    license:            Beerware
    minimumCppStandard: 11
    dependencies:       juce_core

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
#include <juce_core/juce_core.h>

namespace sqlite
{
    #undef SQLITE_64BIT_STATS
    #if JUCE_64BIT
        #define SQLITE_64BIT_STATS 1
    #endif

    #undef SQLITE_DEBUG
    #if JUCE_DEBUG
        #define SQLITE_DEBUG 1
    #endif

    #undef SQLITE_CDECL
    #define SQLITE_CDECL __cdecl

    #undef SQLITE_STDCALL
    #if JUCE_WINDOWS
        #define SQLITE_STDCALL __stdcall
    #endif

    #undef SQLITE_OMIT_AUTOINIT
    #define SQLITE_OMIT_AUTOINIT 1

    #undef SQLITE_THREADSAFE
    #define SQLITE_THREADSAFE 1

    #undef SQLITE_OMIT_LOAD_EXTENSION
    #define SQLITE_OMIT_LOAD_EXTENSION 1

    // Gotta love C programmers...
    #undef MIN
    #undef MAX
    #undef min
    #undef max
    #undef _min
    #undef _max
    #undef __min
    #undef __max
    #undef offsetof
    #undef SWAP

    #include "core/sqlite3ext.h"

    #undef MIN
    #undef MAX
    #undef min
    #undef max
    #undef _min
    #undef _max
    #undef __min
    #undef __max
    #undef offsetof
    #undef SWAP
}

#endif //SQUAREPINE_SQLITE_H
