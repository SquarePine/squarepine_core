SharedObjects::SharedObjects() :
    appSettingsFile (GlobalPaths::getStorageOptions().getDefaultFile()),
    logFile (GlobalPaths::getLogFile()),
    keyPressMappingSet (commandManager)
{
    SQUAREPINE_CRASH_TRACER

    if (appSettingsFile.existsAsFile())
    {
        FileInputStream fis (appSettingsFile);
        appSettings = createValueTreeFromJSON (fis.readEntireStreamAsString(), appSettingsId);
        if (! appSettings.isValid())
            appSettings = ValueTree (appSettingsId);
    }
    else
    {
        const auto r = appSettingsFile.create();
        if (r.failed())
        {
            jassertfalse;
        }
    }

    appTheme                = initProperty (appThemeId, VariantConverter<AppTheme>::toVar (AppTheme::darkTheme));
    windowBounds            = initProperty (windowBoundsId, VariantConverter<Rectangle<int>>::toVar ({}));
    maximised               = initProperty (maximisedId, false);
    audioDeviceSettings     = initProperty (audioDeviceSettingsId, String());

    logger = std::make_unique<FileLogger> (logFile, String());
    Logger::setCurrentLogger (logger.get());

    populate (audioFormatManager);

    reloadAudioDeviceManagerFromSettings();
    languageHandler = std::make_unique<LanguageHandler> (GlobalPaths::getLanguagesFolder());

    using namespace BinaryData;
    lato = std::make_shared<FontFamily> ("Lato", LatoRegular_ttf, LatoRegular_ttfSize);
    jassert (lato != nullptr && lato->isValid());

    lato->regular.thin      = Typeface::createSystemTypefaceFor (LatoThin_ttf,                LatoThin_ttfSize);
    lato->regular.light     = Typeface::createSystemTypefaceFor (LatoLight_ttf,               LatoLight_ttfSize);
    lato->regular.bold      = Typeface::createSystemTypefaceFor (LatoBold_ttf,                LatoBold_ttfSize);
    lato->regular.black     = Typeface::createSystemTypefaceFor (LatoBlack_ttf,               LatoBlack_ttfSize);
    lato->italic.thin       = Typeface::createSystemTypefaceFor (LatoThinItalic_ttf,          LatoThin_ttfSize);
    lato->italic.light      = Typeface::createSystemTypefaceFor (LatoLightItalic_ttf,         LatoLightItalic_ttfSize);
    lato->italic.normal     = Typeface::createSystemTypefaceFor (LatoItalic_ttf,              LatoItalic_ttfSize);
    lato->italic.bold       = Typeface::createSystemTypefaceFor (LatoBoldItalic_ttf,          LatoBoldItalic_ttfSize);
    lato->italic.black      = Typeface::createSystemTypefaceFor (LatoBlackItalic_ttf,         LatoBlackItalic_ttfSize);
    defaultFamily           = lato;

    threadPool = std::make_shared<ThreadPool>();
    imageFormatManager = std::make_shared<ImageFormatManager>();
    imageFormatManager->registerBasicFormats();
}

SharedObjects::~SharedObjects()
{
    SQUAREPINE_CRASH_TRACER

    appSettingsFile.replaceWithText (toJSONString (appSettings));

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

   #if SQUAREPINE_USE_REX_AUDIO_FORMAT
    // afm.registerFormat (new REXAudioFormat(), false); // TODO
   #endif
}

//==============================================================================
bool SharedObjects::isFullscreen() const
{
    return static_cast<bool> (maximised.getValue());
}

void SharedObjects::toggleFullscreen()
{
    maximised.setValue (! isFullscreen());
}

//==============================================================================
void SharedObjects::saveWindowDimensions (const DocumentWindow& dw)
{
    SQUAREPINE_CRASH_TRACER

    windowBounds = VariantConverter<Rectangle<int>>::toVar (dw.getBounds());
    maximised = dw.isFullScreen();
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

    auto rect = VariantConverter<Rectangle<int>>::fromVar (windowBounds);

    if (rect.isEmpty())
    {
        setupWithDefaults();
        return;
    }

    snapRectToCurrentDisplayConfiguration (rect);
    dw.setBounds (rect);
    dw.setFullScreen (static_cast<bool> (maximised.getValue()));
}

std::unique_ptr<Drawable> SharedObjects::getMainLogoSVG()
{
    return svg::Parse::parse (BinaryData::squarepine_logo_colour_svg, BinaryData::squarepine_logo_colour_svgSize);
}

Image SharedObjects::getMainLogoPNG()
{
    return ImageCache::getFromMemory (BinaryData::squarepine_logo_colour_png, BinaryData::squarepine_logo_colour_pngSize);
}

Image SharedObjects::getWindowIcon()
{
    return ImageCache::getFromMemory (BinaryData::squarepine_icon_32_png, BinaryData::squarepine_icon_32_pngSize);
}

Image SharedObjects::getTaskbarIcon()
{
    return ImageCache::getFromMemory (BinaryData::squarepine_trees_icon_24_png, BinaryData::squarepine_trees_icon_24_pngSize);
}

//==============================================================================
Result SharedObjects::saveAudioDeviceSetup()
{
    SQUAREPINE_CRASH_TRACER

    if (auto xml = audioDeviceManager.createStateXml())
    {
        const auto format = XmlElement::TextFormat().singleLine();
        audioDeviceSettings = Base64::toBase64 (xml->toString (format).trim());
        return Result::ok();
    }

    return Result::fail ("");
}

Result SharedObjects::reloadAudioDeviceManagerFromSettings()
{
    SQUAREPINE_CRASH_TRACER

    if (RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        {
            MemoryOutputStream out;
            const auto v = audioDeviceSettings.getValue().toString();
            if (v.isNotEmpty() && Base64::convertFromBase64 (out, v))
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

        const auto result = audioDeviceManager.initialiseWithDefaultDevices (2, 2);
        audioDeviceManager.addAudioDeviceType (std::make_unique<DummyAudioIODeviceType>());

        if (result.isEmpty())
            return Result::ok();

        // No soundcard, driver failed, or something?
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
