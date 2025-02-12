#ifndef SQUAREPINE_DEMO_MAIN_MODULE_H
#define SQUAREPINE_DEMO_MAIN_MODULE_H

#include <JuceHeader.h>

/** Config: SP_DEMO_USE_OPENGL

    Enables or disables setting the application up for OpenGL.

    @see HighPerformanceRendererConfigurator
*/
#ifndef SP_DEMO_USE_OPENGL
    #define SP_DEMO_USE_OPENGL JUCE_MODULE_AVAILABLE_juce_opengl
#endif // SP_DEMO_USE_OPENGL

//==============================================================================
#include "core/GlobalIDs.h"
#include "core/GlobalPathSettings.h"
#include "core/SharedObjects.h"

#include "demos/DemoBase.h"
#include "demos/AnimationDemo.h"
#include "demos/CodeEditorDemo.h"
#include "demos/iCUESDKDemo.h"
#include "demos/EasingsDemo.h"
#include "demos/EffectChainDemo.h"
#include "demos/ImageDemo.h"
#include "demos/MediaDeviceListerDemo.h"
#include "demos/OpenGLDetailsDemo.h"
#include "demos/ParticleSystemDemo.h"
#include "demos/WinRTRGBDemo.h"

#include "components/SettingsComponent.h"
#include "main/DemoLookAndFeel.h"
#include "main/MainComponent.h"

#endif // SQUAREPINE_DEMO_MAIN_MODULE_H
