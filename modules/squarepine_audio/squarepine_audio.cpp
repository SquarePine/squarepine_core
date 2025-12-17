#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1

#undef NOCRYPT
#define NOCRYPT 1

#undef NOGDI
#define NOGDI 1

#undef VC_EXTRALEAN
#define VC_EXTRALEAN 1

#undef JUCE_CORE_INCLUDE_NATIVE_HEADERS
//#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 0

#include "squarepine_audio.h"

#if JUCE_MAC
    #include <CoreFoundation/CoreFoundation.h>
#endif

#if SQUAREPINE_USE_R8BRAIN
    #include <r8brain/r8bbase.cpp>
#endif

#include "linkers/squarepine_AuftaktLinker.cpp"
#include "linkers/squarepine_ElastiqueLinker.cpp"

namespace sp
{
    using namespace juce;

    String getInternalProcessorTypeName()
    {
        return "Internal";
    }

    #include "codecs/squarepine_ALACAudioFormat.cpp"
    #include "codecs/squarepine_REXAudioFormat.cpp"
    #include "core/squarepine_ChildProcessPluginScanner.cpp"
    #include "core/squarepine_EffectProcessor.cpp"
    #include "core/squarepine_EffectProcessorChain.cpp"
    #include "core/squarepine_EffectProcessorFactory.cpp"
    #include "core/squarepine_SquarePineAudioPluginFormat.cpp"
    #include "core/squarepine_InternalProcessor.cpp"
    #include "devices/squarepine_DummyAudioIODevice.cpp"
    #include "devices/squarepine_DummyAudioIODeviceCallback.cpp"
    #include "devices/squarepine_DummyAudioIODeviceType.cpp"
    #include "devices/squarepine_MediaDevicePoller.cpp"
    #include "effects/squarepine_ADSRProcessor.cpp"
    #include "effects/squarepine_BitCrusherProcessor.cpp"
    #include "effects/squarepine_DitherProcessor.cpp"
    #include "effects/squarepine_HissingProcessor.cpp"
    #include "effects/squarepine_LevelsProcessor.cpp"
    #include "effects/squarepine_LFOProcessor.cpp"
    #include "effects/squarepine_MuteProcessor.cpp"
    #include "effects/squarepine_PanProcessor.cpp"
    #include "effects/squarepine_PolarityInversionProcessor.cpp"
    #include "effects/squarepine_SimpleChorusProcessor.cpp"
    #include "effects/squarepine_SimpleCompressorProcessor.cpp"
    #include "effects/squarepine_SimpleDistortionProcessor.cpp"
    #include "effects/squarepine_SimpleEQProcessor.cpp"
    #include "effects/squarepine_SimpleLimiterProcessor.cpp"
    #include "effects/squarepine_SimpleNoiseGateProcessor.cpp"
    #include "effects/squarepine_SimplePhaserProcessor.cpp"
    #include "effects/squarepine_SimpleReverbProcessor.cpp"
    #include "effects/squarepine_StereoWidthProcessor.cpp"
    #include "effects/squarepine_GainProcessor.cpp"
    #include "graphics/squarepine_AudioProcessorGraphEditor.cpp"
    #include "graphics/squarepine_Meter.cpp"
    #include "graphics/squarepine_ProgramAudioProcessorEditor.cpp"
    #include "music/squarepine_Chord.cpp"
    #include "music/squarepine_Pitch.cpp"
    #include "music/squarepine_Scale.cpp"
    // #include "resamplers/squarepine_ElastiqueStretcher.cpp"
    #include "resamplers/squarepine_Resampler.cpp"
    #include "resamplers/squarepine_ResamplingAudioFormatReader.cpp"
    #include "resamplers/squarepine_ResamplingProcessor.cpp"
    #include "resamplers/squarepine_Stretcher.cpp"
    #include "time/squarepine_DecimalTime.cpp"
    #include "time/squarepine_MBTTime.cpp"
    #include "time/squarepine_SMPTETime.cpp"
    #include "time/squarepine_Tempo.cpp"
    #include "time/squarepine_TimeKeeper.cpp"
    #include "time/squarepine_TimeSignature.cpp"
    #include "wrappers/squarepine_AudioSourceProcessor.cpp"
    #include "wrappers/squarepine_AudioTransportProcessor.cpp"
}
