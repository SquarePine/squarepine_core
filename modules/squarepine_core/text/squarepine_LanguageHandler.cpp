//==============================================================================
namespace
{
    const auto languageFileExtension = String (".language");
}

//==============================================================================
LanguageHandler::LanguageHandler (const File& langDir) :
    languageDirectory (langDir)
{
    if (! languageDirectory.isDirectory())
    {
        languageDirectory.deleteRecursively (true);

        const auto r = languageDirectory.createDirectory();

        if (! r.wasOk())
        {
            Logger::writeToLog ("Error! Failed to create language translations directory!\n");

            if (r.getErrorMessage().isNotEmpty())
                Logger::writeToLog ("Message: " + r.getErrorMessage());

            jassertfalse;
            return;
        }
    }

    Array<File> languageFiles;
    languageFiles.minimiseStorageOverheads();
    languageDirectory.findChildFiles (languageFiles, File::findFiles, false, "*" + languageFileExtension);

    IETFLanguageFile ietfDefaultLang;
    const auto defaultLang = languageDirectory.getChildFile (ietfDefaultLang.tag.getDescription() + languageFileExtension);

    if (languageFiles.isEmpty()
        || ! defaultLang.existsAsFile()
        || defaultLang.getSize() < 64
        || defaultLang.isSymbolicLink())
    {
        defaultLang.deleteRecursively (false);

        auto fs = FileOutputStream (defaultLang);
        if (fs.failedToOpen())
        {
            Logger::writeToLog ("Error! Unable to create default language file for some reason...\nSee path:\n\t"  + defaultLang.getFullPathName());
            jassertfalse;
            return;
        }

        if (auto* ls = LocalisedStrings::getCurrentMappings())
        {
            const auto& mappings = ls->getMappings();

            for (const auto& key : mappings.getAllKeys())
                fs << key << " = " << mappings.getValue (key, {});
        }

        ietfDefaultLang.file = defaultLang;
        addLanguageFile (ietfDefaultLang, true);
    } 

    for (const auto& lf : languageFiles)
    {
        IETFLanguageFile f;
        f.file = lf;
        f.tag = IETFLanguageTag::fromString (f.file.getFileNameWithoutExtension());

        if (f.tag.isValid())
        {
            addLanguageFile (f, false);
        }
        else
        {
            Logger::writeToLog ("Error! Unknown or broken language file...\nSee path:\n\t" + lf.getFullPathName());
            jassertfalse;
        }
    }
}

bool LanguageHandler::isValid (const IETFLanguageFile& lf)
{
    return lf.exists() && lf.isValid() && lf.file.getFileExtension() == languageFileExtension;
}

void LanguageHandler::addLanguageFile (const IETFLanguageFile& lf, bool makeActive)
{
    if (! isValid (lf))
    {
        Logger::writeToLog ("Unknown or language file...\nSee path:\n\t"  + lf.file.getFullPathName());
        jassertfalse;
        return;
    }

    if (languages.isEmpty())
        makeActive = true;

    // @TODO: for now, assume the language file is unique...
    languages.add (new IETFLanguageFile (lf));

    if (makeActive)
        setCurrentLanguage (lf);
}

void LanguageHandler::setCurrentLanguage (const IETFLanguageTag& lt)
{
    for (int i = languages.size(); --i >= 0;)
    {
        if (auto* lang = languages.getUnchecked (i))
        {
            if (*lang == lt)
            {
                if (activeLanguageIndex == i)
                    return; //Nothing to do...

                activeLanguageIndex = i;

                refresh();
                return;
            }
        }
    }

    Logger::writeToLog ("Error! Tried to switch to an unknown language: "
                        + String ((int) lt.code) + "-" + String ((int) lt.country));
    jassertfalse;
}

void LanguageHandler::setCurrentLanguage (const IETFLanguageFile& lf)
{
    if (! isValid (lf))
    {
        Logger::writeToLog ("Invalid language file!\nSee path:\n\t"  + lf.file.getFullPathName());
        jassertfalse;
        return;
    }

    for (int i = languages.size(); --i >= 0;)
    {
        if (auto* lang = languages.getUnchecked (i))
        {
            if (*lang == lf)
            {
                setCurrentLanguage (lf.tag);
                return;
            }
        }
    }
}

const IETFLanguageFile& LanguageHandler::getCurrentLanguage() const
{
    jassert (! languages.isEmpty() && activeLanguageIndex >= 0);

    return *languages.getFirst();
}

void LanguageHandler::addListener (Listener* listener)
{
    listeners.add (listener);
    listener->languageChanged (getCurrentLanguage());
}

void LanguageHandler::removeListener (Listener* listener)
{
    listeners.remove (listener);
}

void LanguageHandler::refresh()
{
    const auto& lang = getCurrentLanguage();
    listeners.call ([lang] (Listener& l) { l.languageChanged (lang); });
}
