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
#include "effects/SimpleEQProcessor.cpp"
#include "effects/StereoWidthProcessor.cpp"
//#include "effects/GainProcessor.cpp"
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
#include "effects/daweffects/GainProcessor.cpp"
#include "effects/daweffects/DubEchoProcessor.cpp"
#include "effects/daweffects/CrushProcessor.cpp"
#include "effects/daweffects/InsertProcessor.cpp"
#include "effects/daweffects/DelayProcessor.cpp"
#include "effects/daweffects/BandProcessor.cpp"
#include "effects/daweffects/EchoProcessor.cpp"
#include "effects/daweffects/PingPongProcessor.cpp"
#include "effects/daweffects/SpiralProcessor.cpp"
#include "effects/daweffects/ShimmerProcessor.cpp"
#include "effects/daweffects/ReverbProcessor.cpp"
#include "effects/daweffects/TransEffectProcessor.cpp"
#include "effects/daweffects/LFOFilterProcessor.cpp"
#include "effects/daweffects/FlangerProcessor.cpp"
#include "effects/daweffects/PhaserProcessor.cpp"
#include "effects/daweffects/PitchProcessor.cpp"
#include "effects/daweffects/SlipRollProcessor.cpp"
#include "effects/daweffects/RollProcessor.cpp"
#include "effects/daweffects/VinylBreakProcessor.cpp"
#include "effects/daweffects/HelixProcessor.cpp"
#include "effects/daweffects/BitCrusherProcessor.cpp"
#include "effects/daweffects/NoiseProcessor.cpp"
#include "effects/daweffects/SweepProcessor.cpp"
#include "effects/daweffects/SpaceProcessor.cpp"
#include "effects/daweffects/ShortDelayProcessor.cpp"
#include "effects/daweffects/LongDelayProcessor.cpp"
#include "effects/daweffects/EffectiveTempoProcessor.cpp"
#include "effects/daweffects/VariableBPMProcessor.cpp"
#include "effects/daweffects/EQProcessor.cpp"

}
