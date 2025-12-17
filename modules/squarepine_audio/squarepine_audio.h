#ifndef SQUAREPINE_AUDIO_H
#define SQUAREPINE_AUDIO_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_audio
    vendor:             SquarePine
    version:            1.7.0
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
    #include "core/squarepine_AudioBufferView.h"
    #include "core/squarepine_AudioBufferFIFO.h"
    #include "core/squarepine_AudioUtilities.h"
    #include "core/squarepine_ChildProcessPluginScanner.h"
    #include "core/squarepine_SquarePineAudioPluginFormat.h"
    #include "core/squarepine_InternalProcessor.h"
    #include "effects/squarepine_LevelsProcessor.h"
    class EffectProcessorChain;
    #include "core/squarepine_EffectProcessor.h"
    #include "core/squarepine_EffectProcessorFactory.h"
    #include "core/squarepine_EffectProcessorChain.h"
    #include "core/squarepine_LastKnownPluginDetails.h"
    #include "core/squarepine_MetadataUtilities.h"
    #include "core/squarepine_MIDIChannel.h"
    #include "music/squarepine_Chord.h"
    #include "music/squarepine_Genre.h"
    #include "music/squarepine_Pitch.h"
    #include "music/squarepine_Scale.h"
    #include "codecs/squarepine_ALACAudioFormat.h"
    #include "codecs/squarepine_REXAudioFormat.h"
    #include "devices/squarepine_DummyAudioIODevice.h"
    #include "devices/squarepine_DummyAudioIODeviceCallback.h"
    #include "devices/squarepine_DummyAudioIODeviceType.h"
    #include "devices/squarepine_MediaDevicePoller.h"
    #include "dsp/squarepine_BasicDither.h"
    #include "dsp/squarepine_DistortionFunctions.h"
    #include "dsp/squarepine_EnvelopeFollower.h"
    #include "dsp/squarepine_PositionedImpulseResponse.h"
    #include "effects/squarepine_ADSRProcessor.h"
    #include "effects/squarepine_BitCrusherProcessor.h"
    #include "effects/squarepine_DitherProcessor.h"
    #include "effects/squarepine_HissingProcessor.h"
    #include "effects/squarepine_LFOProcessor.h"
    #include "effects/squarepine_MuteProcessor.h"
    #include "effects/squarepine_PanProcessor.h"
    #include "effects/squarepine_PolarityInversionProcessor.h"
    #include "effects/squarepine_SimpleChorusProcessor.h"
    #include "effects/squarepine_SimpleCompressorProcessor.h"
    #include "effects/squarepine_SimpleDistortionProcessor.h"
    #include "effects/squarepine_SimpleEQProcessor.h"
    #include "effects/squarepine_SimpleLimiterProcessor.h"
    #include "effects/squarepine_SimpleNoiseGateProcessor.h"
    #include "effects/squarepine_SimplePhaserProcessor.h"
    #include "effects/squarepine_SimpleReverbProcessor.h"
    #include "effects/squarepine_StereoWidthProcessor.h"
    #include "effects/squarepine_GainProcessor.h"
    #include "graphics/squarepine_Meter.h"
    #include "graphics/squarepine_ProgramAudioProcessorEditor.h"
    #include "graphics/squarepine_AudioProcessorGraphEditor.h"
    #include "resamplers/squarepine_Resampler.h"
    #include "resamplers/squarepine_ResamplingAudioFormatReader.h"
    #include "resamplers/squarepine_ResamplingProcessor.h"
    #include "resamplers/squarepine_Stretcher.h"
    #include "time/squarepine_TimeHelpers.h"
    #include "time/squarepine_TimeFormat.h"
    #include "time/squarepine_DecimalTime.h"
    #include "time/squarepine_SMPTETime.h"
    #include "time/squarepine_Tempo.h"
    #include "time/squarepine_TimeSignature.h"
    #include "time/squarepine_MBTTime.h"
    #include "time/squarepine_TimeKeeper.h"
    #include "wrappers/squarepine_AudioSourceProcessor.h"
    #include "wrappers/squarepine_AudioTransportProcessor.h"
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
