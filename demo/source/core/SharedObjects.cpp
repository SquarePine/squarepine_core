SharedObjects::SharedObjects() :
    logFile (GlobalPaths::getLogFile()),
    keyPressMappingSet (commandManager)
{
    SQUAREPINE_CRASH_TRACER

    applicationProperties.setStorageParameters (GlobalPaths::getStorageOptions());

    logger = std::make_unique<FileLogger> (logFile, String());
    Logger::setCurrentLogger (logger.get());

    populate (audioFormatManager);

    reloadAudioDeviceManagerFromSettings();
    languageHandler = std::make_unique<LanguageHandler> (GlobalPaths::getLanguagesFolder());
    fullscreen.setValue (false);

    using namespace BinaryData;
    lato = std::make_unique<FontFamily> ("Lato", LatoRegular_ttf, LatoRegular_ttfSize);
    jassert (lato != nullptr && lato->isValid());

    lato->regular.thin         = Typeface::createSystemTypefaceFor (LatoThin_ttf,                LatoThin_ttfSize);
    lato->regular.light        = Typeface::createSystemTypefaceFor (LatoLight_ttf,               LatoLight_ttfSize);
    lato->regular.bold         = Typeface::createSystemTypefaceFor (LatoBold_ttf,                LatoBold_ttfSize);
    lato->regular.black        = Typeface::createSystemTypefaceFor (LatoBlack_ttf,               LatoBlack_ttfSize);
    lato->italic.thin          = Typeface::createSystemTypefaceFor (LatoThinItalic_ttf,          LatoThin_ttfSize);
    lato->italic.light         = Typeface::createSystemTypefaceFor (LatoLightItalic_ttf,         LatoLightItalic_ttfSize);
    lato->italic.normal        = Typeface::createSystemTypefaceFor (LatoItalic_ttf,              LatoItalic_ttfSize);
    lato->italic.bold          = Typeface::createSystemTypefaceFor (LatoBoldItalic_ttf,          LatoBoldItalic_ttfSize);
    lato->italic.black         = Typeface::createSystemTypefaceFor (LatoBlackItalic_ttf,         LatoBlackItalic_ttfSize);

    defaultFamily = lato.get();
}

SharedObjects::~SharedObjects()
{
    SQUAREPINE_CRASH_TRACER

    applicationProperties.saveIfNeeded();

    Logger::setCurrentLogger (nullptr);
}

//==============================================================================
void SharedObjects::clear (bool notifyCommandStatus)
{
    SQUAREPINE_CRASH_TRACER

    commandManager.clearCommands();
    commandManager.setFirstCommandTarget (nullptr);

    if (notifyCommandStatus)
        commandManager.commandStatusChanged();
}

//==============================================================================
void SharedObjects::populate (AudioFormatManager& afm)
{
    afm.registerBasicFormats();
}

//==============================================================================
bool SharedObjects::isFullscreen() const
{
    return static_cast<bool> (fullscreen.getValue());
}

void SharedObjects::toggleFullscreen()
{
    fullscreen.setValue (! isFullscreen());
}

//==============================================================================
void SharedObjects::saveWindowDimensions (const DocumentWindow& dw)
{
    SQUAREPINE_CRASH_TRACER

    if (auto* us = applicationProperties.getUserSettings())
    {
        auto* settingsObject = new DynamicObject();
        settingsObject->setProperty (boundsId, dw.getBounds().toString());
        settingsObject->setProperty (maximisedId, dw.isFullScreen());

        us->setValue (windowSettingsId, JSON::toString (settingsObject, true));
    }
}

void SharedObjects::snapRectToCurrentDisplayConfiguration (Rectangle<int>& b)
{
    SQUAREPINE_CRASH_TRACER

    const auto* primaryDisplay = Desktop::getInstance().getDisplays().getPrimaryDisplay();
    if (primaryDisplay == nullptr)
    {
        b = getDefaultWindowDimensions(); // System appears headless...
        return;
    }

    const auto& mainUserArea = primaryDisplay->userArea;

    // Also check if settings were multi-monitor and if that situation has changed (eg: unplugging the monitor).
    const auto totalArea = Desktop::getInstance().getDisplays().getTotalBounds (false);
    const auto minDims = getMinimumWindowDimensions();

    if (b.getWidth() < minDims.getWidth()
        || b.getHeight() < minDims.getHeight()
        || ! totalArea.contains (b))
    {
        b = getDefaultWindowDimensions().withCentre (mainUserArea.getCentre());
    }
}

void SharedObjects::restoreWindowDimensions (DocumentWindow& dw)
{
    SQUAREPINE_CRASH_TRACER

    auto setupWithDefaults = [&]()
    {
        auto b = getDefaultWindowDimensions();

        if (const auto* primaryDisplay = Desktop::getInstance().getDisplays().getPrimaryDisplay())
            b = b.withCentre (primaryDisplay->userArea.getCentre());

        dw.setBounds (b);
    };

    auto* us = applicationProperties.getUserSettings();
    if (us == nullptr)
    {
        setupWithDefaults();
        return;
    }

    auto settings = us->getValue (windowSettingsId).trim();
    if (settings.isEmpty())
    {
        setupWithDefaults();
        return;
    }

    const auto v = JSON::fromString (settings);

    auto b = Rectangle<int>::fromString (v.getProperty (boundsId, "").toString());
    snapRectToCurrentDisplayConfiguration (b);

    dw.setBounds (b);
    dw.setFullScreen (static_cast<bool> (v.getProperty (maximisedId, false)));
}

Image SharedObjects::getWindowIcon()
{
    return ImageCache::getFromMemory (BinaryData::SquarePine_Logo_png, BinaryData::SquarePine_Logo_pngSize);
}

//==============================================================================
void SharedObjects::saveRecentFiles (const RecentlyOpenedFilesList& rofl)
{
    SQUAREPINE_CRASH_TRACER

    if (auto* us = applicationProperties.getUserSettings())
        us->setValue (recentFilesId, rofl.toString());
}

void SharedObjects::restoreRecentFiles (RecentlyOpenedFilesList& rofl)
{
    if (auto* us = applicationProperties.getUserSettings())
        rofl.restoreFromString (us->getValue (recentFilesId));
}

//==============================================================================
Result SharedObjects::saveAudioDeviceSetup()
{
    SQUAREPINE_CRASH_TRACER

    if (auto* us = applicationProperties.getUserSettings())
    {
        if (auto xml = audioDeviceManager.createStateXml())
        {
            const auto format = XmlElement::TextFormat().singleLine();
            const auto s = Base64::toBase64 (xml->toString (format).trim());
            us->setValue (audioDeviceSettingsId, s);
            return Result::ok();
        }
    }

    return Result::fail ("");
}

Result SharedObjects::reloadAudioDeviceManagerFromSettings()
{
    SQUAREPINE_CRASH_TRACER

    if (RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
#if 0
        if (auto* us = applicationProperties.getUserSettings())
        {
            if (us->containsKey (audioDeviceSettingsId))
            {
                auto s = us->getValue (audioDeviceSettingsId);

                MemoryOutputStream out;
                if (Base64::convertFromBase64 (out, s))
                {
                    if (auto xml = parseXML (out.toString()))
                    {
                        AudioDeviceManager::AudioDeviceSetup setup;
                        setup.sampleRate = 48000.0;
                        setup.bufferSize = 1024;

                        const auto result = audioDeviceManager.initialise (2, 2, xml.get(), true, {}, &setup).trim();
                        if (result.isEmpty())
                            return Result::ok();

                        Logger::writeToLog (result);
                        jassertfalse;
                    }
                }
            }
        }
    #endif

        const auto result = audioDeviceManager.initialiseWithDefaultDevices (128, 128);
        audioDeviceManager.addAudioDeviceType (std::make_unique<DummyAudioIODeviceType>());

        if (result.isEmpty())
            return Result::ok();

        // No soundcard, driver shit the bed, or something?
        Logger::writeToLog (result);
        jassertfalse;
        return Result::fail (result);
    }

    RuntimePermissions::request (RuntimePermissions::recordAudio, [this] (bool wasGranted)
    {
        if (wasGranted)
            MessageManager::callAsync ([this]() { reloadAudioDeviceManagerFromSettings(); });
    });

    return Result::ok();
}

//==============================================================================
void SharedObjects::valueChanged (Value& value)
{
    ignoreUnused (value);

    // if (fullscreen.refersToSameSourceAs (value))
    //    jassertfalse;
}
