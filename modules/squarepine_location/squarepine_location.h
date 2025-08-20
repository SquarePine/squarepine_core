#ifndef SQUAREPINE_LOCATION_H
#define SQUAREPINE_LOCATION_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_location
    vendor:             SquarePine
    version:            1.7.0
    name:               SquarePine Location
    description:        A JUCE module that provides location services and utilities.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 20
    dependencies:       squarepine_core
    OSXFrameworks:      CoreLocation
    iOSFrameworks:      CoreLocation

    END_JUCE_MODULE_DECLARATION
*/

//==============================================================================
#include <squarepine_core/squarepine_core.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "location/GPSLocation.h"
}

#endif // SQUAREPINE_LOCATION_H
