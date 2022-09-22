#ifndef SQUAREPINE_DEMO_MAIN_MODULE_H
#define SQUAREPINE_DEMO_MAIN_MODULE_H

#include <JuceHeader.h>

/** Config: USE_OPENGL
    Enables or disables setting the application up for OpenGL.
*/
#ifndef USE_OPENGL
    #define USE_OPENGL JUCE_MODULE_AVAILABLE_juce_opengl
#endif

namespace
{
    static const char* const trackingId = "UA-000000-0";
}

#include "core/GlobalIDs.h"
#include "core/GlobalPathSettings.h"
#include "core/SharedObjects.h"

#include "demos/DemoBase.h"
#include "demos/CodeEditorDemo.h"
#include "demos/CueSDKDemo.h"
#include "demos/EasingsDemo.h"
#include "demos/ImageDemo.h"
#include "demos/OpenGLDetailsDemo.h"
#include "main/DemoLookAndFeel.h"
#include "main/MainComponent.h"

#endif // SQUAREPINE_DEMO_MAIN_MODULE_H
