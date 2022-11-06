#ifndef SQUAREPINE_ANIMATION_H
#define SQUAREPINE_ANIMATION_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_animation
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine Animation
    description:        A grouping of reusable classes for controlling and managing animations.
    website:            https://www.squarepine.io
    license:            Beerware
    minimumCppStandard: 17
    dependencies:       squarepine_graphics

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_graphics/squarepine_graphics.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "controllers/Timeline.h"
    #include "controllers/TimelineGroup.h"
}

#endif //SQUAREPINE_ANIMATION_H
