#ifndef SQUAREPINE_ANIMATION_H
#define SQUAREPINE_ANIMATION_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_animation
    vendor:             SquarePine
    version:            2.0.0
    name:               SquarePine Animation
    description:        A grouping of reusable classes for controlling and managing animations.
    website:            https://www.squarepine.io
    license:            Beerware
    minimumCppStandard: 17
    dependencies:       squarepine_cryptography squarepine_graphics

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_cryptography/squarepine_cryptography.h>
#include <squarepine_graphics/squarepine_graphics.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "controllers/squarepine_Timeline.h"
    #include "controllers/squarepine_TimelineGroup.h"
    #include "particles/squarepine_ParticleSystem.h"
}

#endif // SQUAREPINE_ANIMATION_H
