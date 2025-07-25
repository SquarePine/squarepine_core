#ifndef SQUAREPINE_ANIMATION_H
#define SQUAREPINE_ANIMATION_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_animation
    vendor:             SquarePine
    version:            1.7.0
    name:               SquarePine Animation
    description:        A grouping of reusable classes for controlling and managing animations.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 17
    dependencies:       juce_animation squarepine_graphics

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_graphics/squarepine_graphics.h>
#include <juce_animation/juce_animation.h>

//==============================================================================
namespace sp
{
    using namespace juce;

    #include "maths/CubicBezier.h"
    #include "maths/Easing.h"
    #include "maths/Spline.h"
    #include "controllers/Timeline.h"
    #include "controllers/TimelineGroup.h"
    #include "particles/ParticleSystem.h"
}

#endif //SQUAREPINE_ANIMATION_H
