# SquarePine JUCE Modules

An assortment of modules that make a great addition to your JUCE app and plugin development tool belt.

# License

The current license is ISC.

Mind you, we'd rather something that requires you to not hide away and privately modify the code. And we'd rather issues and concerns be brought directly here. Till then,   any contributions flowing back this way are much appreciated!

# Library Goal

This library is an assortment of loosely related or unrelated JUCE-based tools, audio or GUI or otherwise, that you can pull up at any given time and with limited hassle. Everything should _"Just Work™"_ with your JUCE project (assuming you're using the latest `develop` branch of JUCE).

We don't care to give ourselves or others more mental overhead - so just drop this stuff into your JUCE projects and be on your way.

With that, we find that dealing with vastly different coding styles, different architectures, different design patterns, and whatever else, to be **exhausting**. So, this repository attempts to make it all fairly consistent, all the while using the same integration strategy (ie: following the JUCE module format).

If you get snagged by anything then let's openly have a look. Got suggestions? Let's have a look. Just make a GitHub Issue, a Pull Request, whatever works. All that we ask is that you try to get on board with the coding standards as best as possible.

The philosophy here is multi-part:

* Easy to integrate into new and existing projects.
* Open up as much code as possible.
* Make the code as generic and flexible as possible.
* Make the code as consistent in design, architecture, and style, as much as possible.
  * Have a look at our coding standards!

# Select Functionality Showcase

* CueSDK support.
* Highly flexible Google Analyitcs integrations.
* A wide array of [easing functions](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/maths/Easing.h) for animation purposes.
* Simplifying font support for look and feels using [`sp::FontFamily`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_graphics/utilities/Fonts.h#L34).
* Translation file notification using [`sp::LanguageHandler`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/text/LanguageHandler.h#L164).
* A highly flexible memory pool allocation system called [`sp::Allocator`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_core/memory/Allocator.h).
* An easy to use metering system with [`sp::LevelProcessor`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/effects/LevelsProcessor.h) and [`sp::Meter`](https://github.com/SquarePine/squarepine_core/blob/main/modules/squarepine_audio/graphics/Meter.h).
