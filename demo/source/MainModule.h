#ifndef SQUAREPINE_DEMO_MAIN_MODULE_H
#define SQUAREPINE_DEMO_MAIN_MODULE_H

#include <JuceHeader.h>

#include <type_traits>

/** Config: SP_DEMO_USE_OPENGL

    Enables or disables setting the application up for OpenGL.

    @see HighPerformanceRendererConfigurator
*/
#ifndef SP_DEMO_USE_OPENGL
    #define SP_DEMO_USE_OPENGL JUCE_MODULE_AVAILABLE_juce_opengl
#endif // SP_DEMO_USE_OPENGL

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
#include "demos/EffectChainDemo.h"
#include "demos/ImageDemo.h"
#include "demos/MediaDeviceListerDemo.h"
#include "demos/OpenGLDetailsDemo.h"

#include "components/SettingsComponent.h"
#include "main/DemoLookAndFeel.h"
#include "main/MainComponent.h"

#endif // SQUAREPINE_DEMO_MAIN_MODULE_H
