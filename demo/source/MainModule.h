#ifndef SQUAREPINE_DEMO_MAIN_MODULE_H
#define SQUAREPINE_DEMO_MAIN_MODULE_H

#include <JuceHeader.h>

/** Config: USE_OPENGL
    Enables or disables setting the application up for OpenGL.
*/
#ifndef USE_OPENGL
    #define USE_OPENGL JUCE_MODULE_AVAILABLE_juce_opengl
#endif

#include "core/GlobalIDs.h"
#include "core/GlobalPathSettings.h"
#include "core/SharedObjects.h"

#include "demos/DemoBase.h"
#include "demos/EasingsDemo.h"
#include "demos/ImageDemo.h"
#include "demos/OpenGLDetailsDemo.h"
#include "main/DemoLookAndFeel.h"
#include "main/MainComponent.h"

#endif // SQUAREPINE_DEMO_MAIN_MODULE_H
