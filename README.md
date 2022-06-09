# SquarePine JUCE Modules

An assortment of modules that make a great addition to your JUCE app and plugin development tool belt.

The philosophy here is multi-part:

* Easy to integrate into new and existing JUCE projects (ie: following the JUCE module format).
* Open up as much code as possible.
* Make the code as generic and flexible as possible.
* Make the code as consistent in design, architecture, and style, as much as possible.
  * Have a look at our coding standards!

This library is an assortment of loosely related or unrelated JUCE-based tools, audio or GUI or otherwise, that you can pull up at any given time and with limited hassle. Everything should _"Just Work™"_ with your JUCE project (assuming you're using the latest `develop` branch of JUCE).

# Functionality Highlights

* [CueSDK](https://github.com/CorsairOfficial/cue-sdk) support for controlling your Corsair devices' RGB lights.
  * You need to install the SDK.
  * Enable [`SQUAREPINE_USE_CUESDK`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/squarepine_graphics.h#L33-L42) when integrating the `squarepine_graphics` module.
  * Use [`sp::corsair::getAllAvailableLEDIDs`](https://github.com/SquarePine/squarepine_core/edit/main/modules/squarepine_graphics/linkers/CueSDKIncluder.h) to find out what's available to control.
  * Call `sp::corsair::fillLED`, `sp::corsair::updateLED`, `sp::corsair::updateAllLEDsWithColour` to have some fun!
* Highly flexible Google Analyitcs integrations by pairing the [`sp::GoogleAnalyticsReporter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L280-L384) with an easy to use and feature extensive [`sp::GoogleAnalyticsMetadata`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/GoogleAnalyticsReporter.h#L3-L277).
* A wide array of [easing functions](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Easing.h) for animation purposes.
* Simplifying font support for look and feels using [`sp::FontWeight`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L5) and [`sp::FontFamily`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L34).
* Translation file notification using [`sp::LanguageHandler`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/text/LanguageHandler.h#L164).
* A highly flexible memory pool allocation system called [`sp::Allocator`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/memory/Allocator.h).
* An easy to use metering system with [`sp::LevelProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LevelsProcessor.h) and [`sp::Meter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/graphics/Meter.h).
* [`sp::NetworkConnectivityChecker`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/networking/NetworkConnectivityChecker.h) will help you find out what kind of network your on, and will give you listener notifications when connectivity changes (connects or disconnects).
* [`sp::ImageFormatManager`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/images/ImageFormatManager.h) makes it easier to bring together your favourite image formats, unifying their instances for loading images with your own codecs using JUCE's [`juce::ImageFileFormat`](https://github.com/juce-framework/JUCE/blob/develop/modules/juce_graphics/images/juce_ImageFileFormat.h) ecosystem.

# License

[The current license is ISC](https://github.com/SquarePine/squarepine_core/blob/main/LICENSE.md).

# Contributing

If you get snagged by anything or if you have suggestions -- let's have a look!

Simply make a GitHub Issue or a Pull Request, whatever's easier for you.
