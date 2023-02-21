# SquarePine JUCE Modules

An assortment of modules that make a great addition to your JUCE app and plugin development tool belt.

The philosophy here is multi-part:

* Easy to integrate into new and existing JUCE projects (ie: following the JUCE module format).
* Open up as much code as possible.
* Make the code as generic and flexible as possible.
* Make the code as consistent in design, architecture, and style, as much as possible.
  * Have a look at our coding standards!

This library is an assortment of loosely related or unrelated JUCE-based tools, audio or GUI or otherwise, all of which you can pull up at any given time and with limited hassle. Everything should _"Just Work™"_ with your JUCE project (assuming you're using the latest `develop` branch of JUCE).

# Functionality Highlights

## `squarepine_core`

* Translation file notification using [`sp::LanguageHandler`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/text/LanguageHandler.h#L164).
* A highly flexible memory pool allocation system called [`sp::Allocator`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/memory/Allocator.h).
* [`sp::NetworkConnectivityChecker`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/NetworkConnectivityChecker.h) will help you find out what kind of network your on, and will give you listener notifications when connectivity changes (connects or disconnects).
* A wide array of math functions and structures, including [`sp::Vector4D`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Vector4D.h) and [`sp::Angle`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Angle.h).
* Many `juce::XYZType` are available wihtin the `juce::ValueTree` ecosystem via the [`VariantConverters`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/valuetree/VariantConverters.h) provided.
* Plenty [freestanding utilities](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/valuetree/ValueTreeHelpers.h) for the `juce::ValueTree` ecosystem.

### Easing Demo

![image](https://user-images.githubusercontent.com/1908886/174856790-680f1e3f-2c82-470d-a107-a590e6646c54.png)

## `squarepine_audio`

### Utilities

1. [`sp::AudioBufferFIFO`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/AudioBufferFIFO.h)
2. [`sp::AudioBufferView`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/AudioBufferView.h)
     * For easy `juce::AudioBuffer` iteration using range-based `for`.
3. [Wide range of straightforward resamplers based on JUCE's interpolators.](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/resamplers/Resampler.h)

### Metering & Graphics

We have an easy to use metering system with [`sp::LevelProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LevelsProcessor.h) and [`sp::Meter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/graphics/Meter.h).

### Many, Many Audio Effects

A vast range of simple renditions of audio effects can be found, including an ecosystem to manage them.

#### The Effects

1. [`sp::BitCrusherProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/BitCrusherProcessor.h)
2. [`sp::ChorusProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/ChorusProcessor.h)
3. [`sp::DitherProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/DitherProcessor.h)
4. [`sp::GainProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/GainProcessor.h)
5. [`sp::HissingProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/HissingProcessor.h)
6. [`sp::JUCEReverbProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/JUCEReverbProcessor.h)
7. [`sp::LFOProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LFOProcessor.h)
8. [`sp::MuteProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/MuteProcessor.h)
9. [`sp::PanProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/PanProcessor.h)
10. [`sp::PolarityInversionProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/PolarityInversionProcessor.h)
11. [`sp::SimpleDistortionProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/SimpleDistortionProcessor.h)
12. [`sp::SimpleEQProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/SimpleEQProcessor.h)
13. [`sp::StereoWidthProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/StereoWidthProcessor.h)

### The Ecosystem

1. [`sp::EffectProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessor.h)
2. [`sp::EffectProcessorFactory`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessorFactory.h)
3. [`sp::EffectProcessorChain`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/EffectProcessorChain.h)
4. [`sp::InternalAudioPluginFormat`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/core/InternalAudioPluginFormat.h)

## `squarepine_graphics`

* [CueSDK](https://github.com/CorsairOfficial/cue-sdk) support for controlling your Corsair devices' RGB lights.
  * You need to install the SDK.
  * Enable [`SQUAREPINE_USE_CUESDK`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/squarepine_graphics.h#L33-L42) when integrating the `squarepine_graphics` module.
  * Use [`sp::corsair::getAllAvailableLEDIDs`](https://github.com/SquarePine/squarepine_core/edit/main/modules/squarepine_graphics/linkers/CueSDKIncluder.h) to find out what's available to control.
  * Call `sp::corsair::fillLED`, `sp::corsair::updateLED`, `sp::corsair::updateAllLEDsWithColour` to have some fun!
* Highly flexible Google Analyitcs integrations by pairing the [`sp::GoogleAnalyticsReporter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L280-L384) with an easy to use and feature extensive [`sp::GoogleAnalyticsMetadata`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L3-L277).
* A wide array of [easing functions](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Easing.h) for animation purposes.
* Simplifying font support for look and feels using [`sp::FontWeight`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L5) and [`sp::FontFamily`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L34).
* [`sp::ImageFormatManager`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/images/ImageFormatManager.h) makes it easier to bring together your favourite image formats, unifying their instances for loading images with your own codecs using JUCE's [`juce::ImageFileFormat`](https://github.com/juce-framework/JUCE/blob/develop/modules/juce_graphics/images/juce_ImageFileFormat.h) ecosystem.

# License

[The current license is Beerware](https://github.com/SquarePine/squarepine_core/blob/main/LICENSE.md).

# Contributing

If you get snagged by anything or if you have suggestions -- let's have a look!

Simply make a GitHub Issue or a Pull Request, whatever's easier for you.
