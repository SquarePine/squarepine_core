# Warning

The [modules will be moving in 2026 to GitLab](https://gitlab.com/squarepine/squarepinemodules), with a [license update to ISC](https://gitlab.com/squarepine/squarepinemodules/-/blob/main/LICENSE).

The intent is to:

* Remove dead code.
* Start fresh.
* Make the license flexible again.
* Prepare it for [JUCE's new Marketplace](https://marketplace.juce.com/products).

# SquarePine Modules

The SquarePine Modules provide a vast array of tools in the [format of JUCE/C++ modules](https://github.com/juce-framework/JUCE/blob/develop/docs/JUCE%20Module%20Format.md). From animation and easing, cryptopgraphy,  networking, and  codecs and audio effects, hopefully you will find something of use!

Everything should _"Just Work™"_ within your JUCE project. If this isn't the case, please see the guidelines under [Contributing](#Contributing).

## Requirements

The C++ modules contained assume you're using [the latest `develop` branch of JUCE](https://github.com/juce-framework/JUCE/tree/develop) and a C++20/modern toolchain.

## Philosophy

The philosophy here is multi-part:

* Easy to integrate into new and existing JUCE projects (ie: following the JUCE module format).
* Open up as much code as possible.
* Make the code as generic and flexible as possible.
* Make the code as consistent in design, architecture, and style, as much as possible.
  * Have a look at [our coding standards](https://github.com/SquarePine/SquarePineCodingStandards/wiki)!

## Shields

[![Version](https://img.shields.io/github/v/release/SquarePine/squarepine_core?color=gold&include_prereleases&label=Release&logo=Github)](https://github.com/SquarePine/squarepine_core/releases/latest)

# Functionality Highlights

## `squarepine_core`

Some major feature highlights:

* Highly flexible Google Analytics integrations
  * Pair up the [`sp::GoogleAnalyticsReporter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L280-L384) with the easy to use [`sp::GoogleAnalyticsMetadata`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L3-L277).
* Translation file management with notifications.
  * [`sp::LanguageHandler`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/text/LanguageHandler.h#L164).
* A highly flexible memory pool allocation system.
  * [`sp::Allocator`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/memory/Allocator.h).
* Find out what kind of network you're on and receive notifications when connectivity changes (connects or disconnects).
  * [`sp::NetworkConnectivityChecker`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/NetworkConnectivityChecker.h) 
* A wide array of math functions and structures.
  * [`sp::Vector4D`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Vector4D.h)
  * [`sp::Angle`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Angle.h)
* Many `juce::XYZType` are available within the `juce::ValueTree` ecosystem via the [`VariantConverters`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/valuetree/VariantConverters.h) provided.
* Plenty [freestanding utilities](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/valuetree/ValueTreeHelpers.h) for the `juce::ValueTree` ecosystem.

## `squarepine_audio`

### Utilities

Some utils to avoid boilerplate and rewriting the same code again:

* [`sp::AudioBufferFIFO`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/AudioBufferFIFO.h)
* [`sp::AudioBufferView`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/AudioBufferView.h)
  * For easy `juce::AudioBuffer` iteration using range-based `for`.
* [Wide range of straightforward resamplers based on JUCE's interpolators.](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/resamplers/Resampler.h)

### Metering & Graphics

We have an easy to use metering system with [`sp::LevelProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LevelsProcessor.h) and [`sp::Meter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/graphics/Meter.h).

### Many, Many Audio Effects

A vast range of simple renditions of audio effects can be found, including an ecosystem to manage them.

#### The Effects

* [`sp::BitCrusherProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/BitCrusherProcessor.h)
* [`sp::ChorusProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/ChorusProcessor.h)
* [`sp::DitherProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/DitherProcessor.h)
* [`sp::GainProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/GainProcessor.h)
* [`sp::HissingProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/HissingProcessor.h)
* [`sp::JUCEReverbProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/JUCEReverbProcessor.h)
* [`sp::LFOProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LFOProcessor.h)
* [`sp::MuteProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/MuteProcessor.h)
* [`sp::PanProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/PanProcessor.h)
* [`sp::PolarityInversionProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/PolarityInversionProcessor.h)
* [`sp::SimpleDistortionProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/SimpleDistortionProcessor.h)
* [`sp::SimpleEQProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/SimpleEQProcessor.h)
* [`sp::StereoWidthProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/StereoWidthProcessor.h)

### The Ecosystem

* [`sp::EffectProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessor.h)
* [`sp::EffectProcessorFactory`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessorFactory.h)
* [`sp::EffectProcessorChain`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessorChain.h)
* [`sp::SquarePineAudioPluginFormat`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/SquarePineAudioPluginFormat.h)

## `squarepine_graphics`

* [iCUESDK](https://github.com/CorsairOfficial/cue-sdk) support for controlling your Corsair devices' RGB lights.
  * You need to have the SDK.
  * Enable [`SQUAREPINE_USE_ICUESDK`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/squarepine_graphics.h#L33-L42) when integrating the `squarepine_graphics` module.
  * Call `sp::corsair::updateAllLEDsWithColour` to have some fun!
* [`sp::ImageFormatManager`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/images/ImageFormatManager.h) makes it easier to bring together your favourite image formats, unifying their instances for loading images with your own codecs using JUCE's [`juce::ImageFileFormat`](https://github.com/juce-framework/JUCE/blob/develop/modules/juce_graphics/images/juce_ImageFileFormat.h) ecosystem.
* Simplifying font support for look and feels using [`sp::FontWeight`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L5) and [`sp::FontFamily`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L34).
* Every standard [easing function](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_animation/maths/Easing.h) for animation purposes.
  * Including a [CubicBezier](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_animation/maths/CubicBezier.h) implementation with its own easing variants.

# Demo Screenshots

## Easing

![image](https://user-images.githubusercontent.com/1908886/174856790-680f1e3f-2c82-470d-a107-a590e6646c54.png)
