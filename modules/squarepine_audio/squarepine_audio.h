#ifndef SQUAREPINE_AUDIO_H
#define SQUAREPINE_AUDIO_H

/** BEGIN_JUCE_MODULE_DECLARATION

    ID:                 squarepine_audio
    vendor:             SquarePine
    version:            0.0.1
    name:               SquarePine Audio
    description:        The audio backbone for any typical audio project.
    website:            https://www.squarepine.io
    license:            Proprietary
    minimumCppStandard: 14
    dependencies:       juce_dsp squarepine_core

    END_JUCE_MODULE_DECLARATION
*/
//==============================================================================
#include <juce_dsp/juce_dsp.h>
#include <squarepine_core/squarepine_core.h>

//==============================================================================
/** Config: SQUAREPINE_USE_R8BRAIN

    Enable this to be able use r8brain as an audio resampler.
*/
#ifndef SQUAREPINE_USE_R8BRAIN
    #define SQUAREPINE_USE_R8BRAIN 0
#endif

/** Config: SQUAREPINE_USE_ELASTIQUE

    Enable this to use zplane's Elastique.

    Only available with MSVC (not MinGW) and macOS.
*/
#ifndef SQUAREPINE_USE_ELASTIQUE
    #define SQUAREPINE_USE_ELASTIQUE 0
#endif

/** Config: SQUAREPINE_USE_AUFTAKT
    Enable this to use zplane's auftakt.

    Only available with MSVC (not MinGW) and macOS.
*/
#ifndef SQUAREPINE_USE_AUFTAKT
    #define SQUAREPINE_USE_AUFTAKT 0
#endif

/** Config: SQUAREPINE_USE_REX_AUDIO_FORMAT

    Enable this to be able to read Propellerheads' REX audio files.

    Only available with MSVC (not MinGW) and macOS (Intel desktop).
*/
#ifndef SQUAREPINE_USE_REX_AUDIO_FORMAT
    #define SQUAREPINE_USE_REX_AUDIO_FORMAT 1
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

//==============================================================================
namespace sp
{
    using namespace juce;

    //==============================================================================
    /** Shorthand for creating shared AudioProcessor instances. */
    using AudioProcessorPtr = std::shared_ptr<AudioProcessor>;
    /** Shorthand for creating weak pointers to AudioProcessor instances. */
    using AudioProcessorWeakPtr = std::weak_ptr<AudioProcessor>;

    /** Shorthand for creating shared AudioDeviceManager instances. */
    using AudioDeviceManagerPtr = std::shared_ptr<AudioDeviceManager>;
    /** Shorthand for creating weak pointers to AudioDeviceManager instances. */
    using AudioDeviceManagerWeakPtr = std::shared_ptr<AudioDeviceManager>;

    //==============================================================================
    /** */
    String getInternalProcessorTypeName();

    //==============================================================================
    #include "core/AudioBufferView.h"
    #include "core/AudioBufferFIFO.h"
    #include "core/AudioParameterDouble.h"
    #include "core/AudioUtilities.h"
    #include "core/ChildProcessPluginScanner.h"
    #include "core/InternalAudioPluginFormat.h"
    #include "core/InternalProcessor.h"
    #include "core/EffectProcessor.h"
    #include "core/EffectProcessorFactory.h"
    #include "core/EffectProcessorChain.h"
    #include "core/MetadataUtilities.h"
    #include "core/MIDIChannel.h"
    #include "codecs/REXAudioFormat.h"
    #include "devices/DummyAudioIODevice.h"
    #include "devices/DummyAudioIODeviceCallback.h"
    #include "devices/DummyAudioIODeviceType.h"
    #include "devices/MediaDevicePoller.h"
    #include "dsp/BasicDither.h"
    #include "dsp/DistortionFunctions.h"
    #include "dsp/EnvelopeFollower.h"
    #include "dsp/LFO.h"
    #include "dsp/PositionedImpulseResponse.h"
    #include "effects/ADSRProcessor.h"
    #include "effects/BitCrusherProcessor.h"
    #include "effects/ChorusProcessor.h"
    #include "effects/DitherProcessor.h"
    #include "effects/HissingProcessor.h"
    #include "effects/JUCEReverbProcessor.h"
    #include "effects/LevelsProcessor.h"
    #include "effects/LFOProcessor.h"
    #include "effects/MuteProcessor.h"
    #include "effects/PanProcessor.h"
    #include "effects/PolarityInversionProcessor.h"
    #include "effects/SimpleDistortionProcessor.h"
    #include "effects/StereoWidthProcessor.h"
    #include "effects/VolumeProcessor.h"
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
    struct VariantConverter<LevelsProcessor::Mode>
    {
        /** */
        using Mode = LevelsProcessor::Mode;

        /** */
        static Mode fromVar (const var& v) { return (Mode) static_cast<int> (v); }
        /** */
        static var toVar (Mode mode)       { return static_cast<int> (mode); }
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
                    return { static_cast<int> (data->getFirst()),
                             static_cast<int> (data->getLast()) };
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
            Array<var> vals = { t.numerator, t.denominator };
            return vals;
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
                        PluginDescription pd;
                        if (pd.loadFromXml (*xml))
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
            auto xml = pd.createXml();
            jassert (xml != nullptr);
            return Base64::toBase64 (xml->toString());
        }
    };
}

#endif //SQUAREPINE_AUDIO_H
