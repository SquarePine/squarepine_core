/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    There's a section below where you can add your own custom code safely, and the
    Projucer will preserve the contents of that block, but the best way to change
    any of these definitions is by using the Projucer's project settings.

    Any commented-out settings will assume their default values.

*/

#pragma once

//==============================================================================
// [BEGIN_USER_CODE_SECTION]

// (You can add your own code in this section, and the Projucer will not overwrite it)

// [END_USER_CODE_SECTION]

#define JUCE_PROJUCER_VERSION 0x8000a

//==============================================================================
#define JUCE_MODULE_AVAILABLE_juce_analytics                      1
#define JUCE_MODULE_AVAILABLE_juce_animation                      1
#define JUCE_MODULE_AVAILABLE_juce_audio_basics                   1
#define JUCE_MODULE_AVAILABLE_juce_audio_devices                  1
#define JUCE_MODULE_AVAILABLE_juce_audio_formats                  1
#define JUCE_MODULE_AVAILABLE_juce_audio_processors               1
#define JUCE_MODULE_AVAILABLE_juce_audio_processors_headless      1
#define JUCE_MODULE_AVAILABLE_juce_audio_utils                    1
#define JUCE_MODULE_AVAILABLE_juce_box2d                          1
#define JUCE_MODULE_AVAILABLE_juce_core                           1
#define JUCE_MODULE_AVAILABLE_juce_cryptography                   1
#define JUCE_MODULE_AVAILABLE_juce_data_structures                1
#define JUCE_MODULE_AVAILABLE_juce_dsp                            1
#define JUCE_MODULE_AVAILABLE_juce_events                         1
#define JUCE_MODULE_AVAILABLE_juce_graphics                       1
#define JUCE_MODULE_AVAILABLE_juce_gui_basics                     1
#define JUCE_MODULE_AVAILABLE_juce_gui_extra                      1
#define JUCE_MODULE_AVAILABLE_juce_javascript                     1
#define JUCE_MODULE_AVAILABLE_juce_midi_ci                        1
#define JUCE_MODULE_AVAILABLE_juce_opengl                         1
#define JUCE_MODULE_AVAILABLE_juce_osc                            1
#define JUCE_MODULE_AVAILABLE_juce_product_unlocking              1
#define JUCE_MODULE_AVAILABLE_juce_video                          1
#define JUCE_MODULE_AVAILABLE_squarepine_animation                1
#define JUCE_MODULE_AVAILABLE_squarepine_audio                    1
#define JUCE_MODULE_AVAILABLE_squarepine_core                     1
#define JUCE_MODULE_AVAILABLE_squarepine_cryptography             1
#define JUCE_MODULE_AVAILABLE_squarepine_graphics                 1

#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1

//==============================================================================
// juce_audio_devices flags:

#ifndef    JUCE_USE_WINRT_MIDI
 #define   JUCE_USE_WINRT_MIDI 1
#endif

#ifndef    JUCE_USE_WINDOWS_MIDI_SERVICES
 //#define JUCE_USE_WINDOWS_MIDI_SERVICES 0
#endif

#ifndef    JUCE_ASIO
 //#define JUCE_ASIO 0
#endif

#ifndef    JUCE_WASAPI
 //#define JUCE_WASAPI 1
#endif

#ifndef    JUCE_DIRECTSOUND
 //#define JUCE_DIRECTSOUND 1
#endif

#ifndef    JUCE_ALSA
 //#define JUCE_ALSA 1
#endif

#ifndef    JUCE_JACK
 //#define JUCE_JACK 0
#endif

#ifndef    JUCE_USE_ANDROID_OBOE
 //#define JUCE_USE_ANDROID_OBOE 1
#endif

#ifndef    JUCE_USE_OBOE_STABILIZED_CALLBACK
 //#define JUCE_USE_OBOE_STABILIZED_CALLBACK 0
#endif

#ifndef    JUCE_USE_ANDROID_OPENSLES
 //#define JUCE_USE_ANDROID_OPENSLES 0
#endif

#ifndef    JUCE_DISABLE_AUDIO_MIXING_WITH_OTHER_APPS
 //#define JUCE_DISABLE_AUDIO_MIXING_WITH_OTHER_APPS 0
#endif

//==============================================================================
// juce_audio_formats flags:

#ifndef    JUCE_USE_FLAC
 //#define JUCE_USE_FLAC 1
#endif

#ifndef    JUCE_USE_OGGVORBIS
 //#define JUCE_USE_OGGVORBIS 1
#endif

#ifndef    JUCE_USE_MP3AUDIOFORMAT
 #define   JUCE_USE_MP3AUDIOFORMAT 1
#endif

#ifndef    JUCE_USE_LAME_AUDIO_FORMAT
 //#define JUCE_USE_LAME_AUDIO_FORMAT 0
#endif

#ifndef    JUCE_USE_WINDOWS_MEDIA_FORMAT
 //#define JUCE_USE_WINDOWS_MEDIA_FORMAT 1
#endif

//==============================================================================
// juce_audio_processors_headless flags:

#ifndef    JUCE_PLUGINHOST_VST
 //#define JUCE_PLUGINHOST_VST 0
#endif

#ifndef    JUCE_PLUGINHOST_VST3
 #define   JUCE_PLUGINHOST_VST3 1
#endif

#ifndef    JUCE_PLUGINHOST_AU
 #define   JUCE_PLUGINHOST_AU 1
#endif

#ifndef    JUCE_PLUGINHOST_LADSPA
 #define   JUCE_PLUGINHOST_LADSPA 1
#endif

#ifndef    JUCE_PLUGINHOST_LV2
 //#define JUCE_PLUGINHOST_LV2 0
#endif

#ifndef    JUCE_PLUGINHOST_ARA
 //#define JUCE_PLUGINHOST_ARA 0
#endif

#ifndef    JUCE_CUSTOM_VST3_SDK
 //#define JUCE_CUSTOM_VST3_SDK 0
#endif

//==============================================================================
// juce_audio_utils flags:

#ifndef    JUCE_USE_CDREADER
 //#define JUCE_USE_CDREADER 0
#endif

#ifndef    JUCE_USE_CDBURNER
 //#define JUCE_USE_CDBURNER 0
#endif

//==============================================================================
// juce_core flags:

#ifndef    JUCE_FORCE_DEBUG
 //#define JUCE_FORCE_DEBUG 0
#endif

#ifndef    JUCE_LOG_ASSERTIONS
 //#define JUCE_LOG_ASSERTIONS 0
#endif

#ifndef    JUCE_CHECK_MEMORY_LEAKS
 //#define JUCE_CHECK_MEMORY_LEAKS 1
#endif

#ifndef    JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES
 //#define JUCE_DONT_AUTOLINK_TO_WIN32_LIBRARIES 0
#endif

#ifndef    JUCE_INCLUDE_ZLIB_CODE
 //#define JUCE_INCLUDE_ZLIB_CODE 1
#endif

#ifndef    JUCE_USE_CURL
 //#define JUCE_USE_CURL 1
#endif

#ifndef    JUCE_LOAD_CURL_SYMBOLS_LAZILY
 //#define JUCE_LOAD_CURL_SYMBOLS_LAZILY 0
#endif

#ifndef    JUCE_CATCH_UNHANDLED_EXCEPTIONS
 //#define JUCE_CATCH_UNHANDLED_EXCEPTIONS 0
#endif

#ifndef    JUCE_ALLOW_STATIC_NULL_VARIABLES
 //#define JUCE_ALLOW_STATIC_NULL_VARIABLES 0
#endif

#ifndef    JUCE_STRICT_REFCOUNTEDPOINTER
 #define   JUCE_STRICT_REFCOUNTEDPOINTER 1
#endif

#ifndef    JUCE_ENABLE_ALLOCATION_HOOKS
 //#define JUCE_ENABLE_ALLOCATION_HOOKS 0
#endif

//==============================================================================
// juce_dsp flags:

#ifndef    JUCE_ASSERTION_FIRFILTER
 //#define JUCE_ASSERTION_FIRFILTER 1
#endif

#ifndef    JUCE_DSP_USE_INTEL_MKL
 //#define JUCE_DSP_USE_INTEL_MKL 0
#endif

#ifndef    JUCE_DSP_USE_SHARED_FFTW
 //#define JUCE_DSP_USE_SHARED_FFTW 0
#endif

#ifndef    JUCE_DSP_USE_STATIC_FFTW
 //#define JUCE_DSP_USE_STATIC_FFTW 0
#endif

#ifndef    JUCE_DSP_ENABLE_SNAP_TO_ZERO
 //#define JUCE_DSP_ENABLE_SNAP_TO_ZERO 1
#endif

//==============================================================================
// juce_events flags:

#ifndef    JUCE_EXECUTE_APP_SUSPEND_ON_BACKGROUND_TASK
 //#define JUCE_EXECUTE_APP_SUSPEND_ON_BACKGROUND_TASK 0
#endif

//==============================================================================
// juce_graphics flags:

#ifndef    JUCE_USE_COREIMAGE_LOADER
 //#define JUCE_USE_COREIMAGE_LOADER 1
#endif

#ifndef    JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING
 //#define JUCE_DISABLE_COREGRAPHICS_FONT_SMOOTHING 0
#endif

//==============================================================================
// juce_gui_basics flags:

#ifndef    JUCE_ENABLE_REPAINT_DEBUGGING
 //#define JUCE_ENABLE_REPAINT_DEBUGGING 0
#endif

#ifndef    JUCE_USE_XRANDR
 //#define JUCE_USE_XRANDR 1
#endif

#ifndef    JUCE_USE_XINERAMA
 //#define JUCE_USE_XINERAMA 1
#endif

#ifndef    JUCE_USE_XSHM
 //#define JUCE_USE_XSHM 1
#endif

#ifndef    JUCE_USE_XRENDER
 //#define JUCE_USE_XRENDER 0
#endif

#ifndef    JUCE_USE_XCURSOR
 //#define JUCE_USE_XCURSOR 1
#endif

#ifndef    JUCE_WIN_PER_MONITOR_DPI_AWARE
 //#define JUCE_WIN_PER_MONITOR_DPI_AWARE 1
#endif

//==============================================================================
// juce_gui_extra flags:

#ifndef    JUCE_WEB_BROWSER
 //#define JUCE_WEB_BROWSER 1
#endif

#ifndef    JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING
 //#define JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING 0
#endif

#ifndef    JUCE_USE_WIN_WEBVIEW2
 #define   JUCE_USE_WIN_WEBVIEW2 1
#endif

#ifndef    JUCE_ENABLE_LIVE_CONSTANT_EDITOR
 //#define JUCE_ENABLE_LIVE_CONSTANT_EDITOR 0
#endif

//==============================================================================
// juce_video flags:

#ifndef    JUCE_USE_CAMERA
 #define   JUCE_USE_CAMERA 1
#endif

#ifndef    JUCE_SYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME
 //#define JUCE_SYNC_VIDEO_VOLUME_WITH_OS_MEDIA_VOLUME 1
#endif

//==============================================================================
// squarepine_audio flags:

#ifndef    SQUAREPINE_USE_R8BRAIN
 //#define SQUAREPINE_USE_R8BRAIN 0
#endif

#ifndef    SQUAREPINE_USE_ELASTIQUE
 //#define SQUAREPINE_USE_ELASTIQUE 0
#endif

#ifndef    SQUAREPINE_USE_AUFTAKT
 //#define SQUAREPINE_USE_AUFTAKT 0
#endif

#ifndef    SQUAREPINE_USE_REX_AUDIO_FORMAT
 #define   SQUAREPINE_USE_REX_AUDIO_FORMAT 1
#endif

//==============================================================================
// squarepine_core flags:

#ifndef    SQUAREPINE_COMPILE_UNIT_TESTS
 #define   SQUAREPINE_COMPILE_UNIT_TESTS 1
#endif

#ifndef    SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR
 //#define SQUAREPINE_ARRAY_ITERATION_UNROLLER_MAKE_LINEAR 0
#endif

#ifndef    SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS
 //#define SQUAREPINE_ARRAY_ITERATION_UNROLLER_CHECK_BIG_NUMS 0
#endif

#ifndef    SQUAREPINE_LOG_NETWORK_CALLS
 //#define SQUAREPINE_LOG_NETWORK_CALLS 1
#endif

#ifndef    SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS
 //#define SQUAREPINE_AUTOCONFIG_MAIN_THREAD_LOG_FILTERS 0
#endif

#ifndef    SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE
 //#define SQUAREPINE_AUTOLOG_FUNCTION_AND_LINE 0
#endif

#ifndef    SQUAREPINE_USE_GOOGLE_ANALYTICS
 //#define SQUAREPINE_USE_GOOGLE_ANALYTICS 1
#endif

#ifndef    SQUAREPINE_LOG_GOOGLE_ANALYTICS
 #define   SQUAREPINE_LOG_GOOGLE_ANALYTICS 1
#endif

#ifndef    SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS
 //#define SQUAREPINE_ONLY_LOG_GOOGLE_ANALYTICS 0
#endif

//==============================================================================
// squarepine_graphics flags:

#ifndef    SQUAREPINE_USE_AVIR_RESIZER
 //#define SQUAREPINE_USE_AVIR_RESIZER 1
#endif

#ifndef    SQUAREPINE_USE_ICUESDK
 //#define SQUAREPINE_USE_ICUESDK 0
#endif

#ifndef    SQUAREPINE_LOG_OPENGL_INFO
 #define   SQUAREPINE_LOG_OPENGL_INFO 1
#endif

#ifndef    SQUAREPINE_USE_WINRTRGB
 //#define SQUAREPINE_USE_WINRTRGB 1
#endif

//==============================================================================
#ifndef    JUCE_STANDALONE_APPLICATION
 #if defined(JucePlugin_Name) && defined(JucePlugin_Build_Standalone)
  #define  JUCE_STANDALONE_APPLICATION JucePlugin_Build_Standalone
 #else
  #define  JUCE_STANDALONE_APPLICATION 1
 #endif
#endif
