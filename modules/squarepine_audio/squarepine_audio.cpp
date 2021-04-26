#undef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1

#undef NOCRYPT
#define NOCRYPT 1

#undef NOGDI
#define NOGDI 1

#undef VC_EXTRALEAN
#define VC_EXTRALEAN 1

#undef JUCE_CORE_INCLUDE_NATIVE_HEADERS
#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1

#undef JUCE_CORE_INCLUDE_OBJC_HELPERS
#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1

#include "squarepine_audio.h"

#if SQUAREPINE_USE_R8BRAIN
    #include <r8brain/r8bbase.cpp>
#endif

#include "linkers/AuftaktLinker.cpp"
#include "linkers/ElastiqueLinker.cpp"

namespace sp
{
    using namespace juce;

    String getInternalProcessorTypeName()
    {
        return "Internal";
    }
    
    #include "codecs/REXAudioFormat.cpp"
    #include "core/AudioParameterDouble.cpp"
    #include "core/ChildProcessPluginScanner.cpp"
    #include "core/EffectProcessor.cpp"
    #include "core/EffectProcessorChain.cpp"
    #include "core/EffectProcessorFactory.cpp"
    #include "core/InternalAudioPluginFormat.cpp"
    #include "core/InternalProcessor.cpp"
    #include "devices/DummyAudioIODevice.cpp"
    #include "devices/DummyAudioIODeviceCallback.cpp"
    #include "devices/DummyAudioIODeviceType.cpp"
    #include "devices/MediaDevicePoller.cpp"
    #include "dsp/LFO.cpp"
    #include "effects/ADSRProcessor.cpp"
    #include "effects/BitCrusherProcessor.cpp"
    #include "effects/ChorusProcessor.cpp"
    #include "effects/DitherProcessor.cpp"
    #include "effects/HissingProcessor.cpp"
    #include "effects/JUCEReverbProcessor.cpp"
    #include "effects/LevelsProcessor.cpp"
    #include "effects/LFOProcessor.cpp"
    #include "effects/MuteProcessor.cpp"
    #include "effects/PanProcessor.cpp"
    #include "effects/PolarityInversionProcessor.cpp"
    #include "effects/SimpleDistortionProcessor.cpp"
    #include "effects/StereoWidthProcessor.cpp"
    #include "effects/VolumeProcessor.cpp"
    #include "graphics/GraphObserver.cpp"
    #include "graphics/Meter.cpp"
    #include "graphics/ProgramAudioProcessorEditor.cpp"
    #include "music/Chord.cpp"
    #include "music/Pitch.cpp"
    #include "music/Scale.cpp"
    // #include "resamplers/ElastiqueStretcher.cpp"
    #include "resamplers/Resampler.cpp"
    #include "resamplers/ResamplingAudioFormatReader.cpp"
    #include "resamplers/ResamplingProcessor.cpp"
    #include "resamplers/Stretcher.cpp"
    #include "time/DecimalTime.cpp"
    #include "time/MBTTime.cpp"
    #include "time/SMPTETime.cpp"
    #include "time/Tempo.cpp"
    #include "time/TimeKeeper.cpp"
    #include "time/TimeSignature.cpp"
    #include "wrappers/AudioSourceProcessor.cpp"
    #include "wrappers/AudioTransportProcessor.cpp"
}
