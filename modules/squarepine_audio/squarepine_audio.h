#ifndef SQUAREPINE_AUDIO_H
#define SQUAREPINE_AUDIO_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_audio
    vendor:             SquarePine
    version:            1.6.0
    name:               SquarePine Audio
    description:        A great backbone for any typical audio project.
    website:            https://www.squarepine.io
    license:            GPLv3
    minimumCppStandard: 20
    dependencies:       squarepine_cryptography juce_dsp

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <squarepine_core/squarepine_core.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/** Config: SQUAREPINE_USE_R8BRAIN

    Enable this to be able use r8brain as an audio resampler.
*/
#ifndef SQUAREPINE_USE_R8BRAIN
    #define SQUAREPINE_USE_R8BRAIN 0
#endif

/** Config: SQUAREPINE_USE_ELASTIQUE

    Enable this to use zplane's Elastique.

    Only available with MSVC and macOS.
*/
#ifndef SQUAREPINE_USE_ELASTIQUE
    #define SQUAREPINE_USE_ELASTIQUE 0
#endif

/** Config: SQUAREPINE_USE_AUFTAKT
    Enable this to help you use zplane's Auftakt.

    Only available with MSVC and macOS.
*/
#ifndef SQUAREPINE_USE_AUFTAKT
    #define SQUAREPINE_USE_AUFTAKT 0
#endif

/** Config: SQUAREPINE_USE_REX_AUDIO_FORMAT

    Enable this to be able to read Propellerheads' REX audio files.

    Only available with MSVC and macOS (Intel desktop).
*/
#ifndef SQUAREPINE_USE_REX_AUDIO_FORMAT
    #define SQUAREPINE_USE_REX_AUDIO_FORMAT 0
#endif

//==============================================================================
// Incomplete support right now...
#undef SQUAREPINE_USE_R8BRAIN

#if SQUAREPINE_USE_R8BRAIN
    #include <r8brain/CDSPBlockConvolver.h>
    #include <r8brain/CDSPFIRFilter.h>
    #include <r8brain/CDSPFracInterpolator.h>
    #include <r8brain/CDSPProcessor.h>
    #include <r8brain/CDSPRealFFT.h>
    #include <r8brain/CDSPResampler.h>
    #include <r8brain/CDSPSincFilterGen.h>
    #include <r8brain/fft4g.h>
    #include <r8brain/HeapFifo.h>
    #include <r8brain/r8bbase.h>
    #include <r8brain/r8bconf.h>
    #include <r8brain/r8butil.h>
#endif

//==============================================================================
#include "linkers/ZplaneHelpers.h"

namespace juce
{
    /** @returns true if the lhs is a duplicate of the rhs.
        JUCE is a bit funny about missing this function. I guess nobody complained enough...
    */
    inline bool operator== (const PluginDescription& lhs, const PluginDescription& rhs)
    {
        return lhs.isDuplicateOf (rhs); 
    }

    /** @returns true if the lhs is not a duplicate of the rhs.
        JUCE is a bit funny about missing this function. I guess nobody complained enough...
    */
    inline bool operator!= (const PluginDescription& lhs, const PluginDescription& rhs)
    {
        return ! operator== (lhs, rhs);
    }
}

//==============================================================================
namespace sp
{
    using namespace juce;

    //==============================================================================
    /** Shorthand for creating shared AudioProcessor instances. */
    using AudioProcessorPtr = std::shared_ptr<AudioProcessor>;
    /** Shorthand for creating weak pointers to AudioProcessor instances. */
    using AudioProcessorWeakPtr = std::weak_ptr<AudioProcessor>;

    /** Shorthand for creating shared AudioPluginInstance instances. */
    using AudioPluginPtr = std::shared_ptr<AudioPluginInstance>;
    /** Shorthand for creating weak pointers to AudioPluginInstance instances. */
    using AudioPluginWeakPtr = std::weak_ptr<AudioPluginInstance>;

    /** Shorthand for creating shared AudioDeviceManager instances. */
    using AudioDeviceManagerPtr = std::shared_ptr<AudioDeviceManager>;
    /** Shorthand for creating weak pointers to AudioDeviceManager instances. */
    using AudioDeviceManagerWeakPtr = std::weak_ptr<AudioDeviceManager>;

    //==============================================================================
    /** */
    String getInternalProcessorTypeName();

    //==============================================================================
    #include "core/AudioBufferView.h"
    #include "core/AudioBufferFIFO.h"
    #include "core/AudioUtilities.h"
    #include "core/ChildProcessPluginScanner.h"
    #include "core/SquarePineAudioPluginFormat.h"
    #include "core/InternalProcessor.h"
    #include "effects/LevelsProcessor.h"
    class EffectProcessorChain;
    #include "core/EffectProcessor.h"
    #include "core/EffectProcessorFactory.h"
    #include "core/EffectProcessorChain.h"
    #include "core/LastKnownPluginDetails.h"
    #include "core/MetadataUtilities.h"
    #include "core/MIDIChannel.h"
    #include "codecs/ALACAudioFormat.h"
    #include "codecs/REXAudioFormat.h"
    #include "devices/DummyAudioIODevice.h"
    #include "devices/DummyAudioIODeviceCallback.h"
    #include "devices/DummyAudioIODeviceType.h"
    #include "devices/MediaDevicePoller.h"
    #include "dsp/BasicDither.h"
    #include "dsp/DistortionFunctions.h"
    #include "dsp/EnvelopeFollower.h"
    #include "dsp/PositionedImpulseResponse.h"
    #include "effects/ADSRProcessor.h"
    #include "effects/BitCrusherProcessor.h"
    #include "effects/DitherProcessor.h"
    #include "effects/HissingProcessor.h"
    #include "effects/LFOProcessor.h"
    #include "effects/MuteProcessor.h"
    #include "effects/PanProcessor.h"
    #include "effects/PolarityInversionProcessor.h"
    #include "effects/SimpleChorusProcessor.h"
    #include "effects/SimpleCompressorProcessor.h"
    #include "effects/SimpleDistortionProcessor.h"
    #include "effects/SimpleEQProcessor.h"
    #include "effects/SimpleLimiterProcessor.h"
    #include "effects/SimpleNoiseGateProcessor.h"
    #include "effects/SimplePhaserProcessor.h"
    #include "effects/SimpleReverbProcessor.h"
    #include "effects/StereoWidthProcessor.h"
    #include "effects/GainProcessor.h"
    #include "graphics/GraphObserver.h"
    #include "graphics/Meter.h"
    #include "graphics/ProgramAudioProcessorEditor.h"
    #include "music/Chord.h"
    #include "music/Genre.h"
    #include "music/Pitch.h"
    #include "music/Scale.h"
    #include "resamplers/Resampler.h"
    #include "resamplers/ResamplingAudioFormatReader.h"
    #include "resamplers/ResamplingProcessor.h"
    #include "resamplers/Stretcher.h"
    #include "time/TimeHelpers.h"
    #include "time/TimeFormat.h"
    #include "time/DecimalTime.h"
    #include "time/SMPTETime.h"
    #include "time/Tempo.h"
    #include "time/TimeSignature.h"
    #include "time/MBTTime.h"
    #include "time/TimeKeeper.h"
    #include "wrappers/AudioSourceProcessor.h"
    #include "wrappers/AudioTransportProcessor.h"
}

//==============================================================================
namespace juce
{
    using namespace sp;

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<MeteringMode>
    {
        /** */
        static MeteringMode fromVar (const var& v)  { return (MeteringMode) static_cast<int> (v); }
        /** */
        static var toVar (MeteringMode mode)        { return static_cast<int> (mode); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<TimeSignature> final
    {
        /** */
        static TimeSignature fromVar (const var& v)
        {
            if (auto* data = v.getArray())
            {
                if (data->size() == 2)
                {
                    return
                    {
                        static_cast<int> (data->getFirst()),
                        static_cast<int> (data->getLast())
                    };
                }
            }

            jassertfalse;
            return {};
        }

        /** @returns the following as JSON compatible array notation:
            @code
                [ 4, 4 ]
            @endcode
        */
        static var toVar (const TimeSignature& t)
        {
            return Array<var> { t.numerator, t.denominator };
        }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<Tempo> final
    {
        /** */
        static Tempo fromVar (const var& v) noexcept { return Tempo (static_cast<double> (v)); }
        /** */
        static var toVar (const Tempo& t) noexcept   { return t.get(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<MIDIChannel> final
    {
        /** */
        static MIDIChannel fromVar (const var& v) noexcept  { return MIDIChannel (static_cast<int> (v)); }
        /** */
        static var toVar (const MIDIChannel& mc) noexcept   { return mc.get(); }
    };

    //==============================================================================
    /** */
    template<>
    struct VariantConverter<PluginDescription> final
    {
        /** */
        static PluginDescription fromVar (const var& v)
        {
            if (v.isString())
            {
                MemoryOutputStream mos;
                if (Base64::convertFromBase64 (mos, v.toString()))
                {
                    if (auto xml = parseXML (mos.toString()))
                    {
                        if (PluginDescription pd; pd.loadFromXml (*xml))
                            return pd;

                        jassertfalse;
                    }
                    else
                    {
                        jassertfalse;
                    }
                }
                else
                {
                    jassertfalse;
                }
            }
            else
            {
                jassertfalse;
            }

            return {};
        }

        /** @returns a PluginDescription as Base64 XML data. */
        static var toVar (const PluginDescription& pd)
        {
            if (auto xml = pd.createXml())
                return Base64::toBase64 (xml->toString());

            jassertfalse;
            return {};
        }
    };
}

#endif //SQUAREPINE_AUDIO_H
