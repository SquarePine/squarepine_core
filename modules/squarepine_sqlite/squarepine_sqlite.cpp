#ifdef SQUAREPINE_SQLITE_H
    /** When you add this cpp file to your project, you mustn't include it in a file where you've
        already included any other headers - just put it inside a file on its own, possibly with your config
        flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
        header files that the compiler may be using.
    */
    #error "Incorrect use of JUCE cpp file"
#endif

#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1

#include "squarepine_sqlite.h"

namespace sqlite
{
    #include "core/sqlite3.c"
}
