#pragma once

namespace GlobalPaths
{
    /** @returns */
    inline PropertiesFile::Options getStorageOptions()
    {
        PropertiesFile::Options options;
        options.applicationName = ProjectInfo::projectName;
        options.filenameSuffix = ".txt";
        options.folderName = ProjectInfo::companyName;
        options.osxLibrarySubFolder = "Application Support";
        options.doNotSave = false;

        auto replaceSpacesWithUnderscore = [] (String& s)
        {
            s = s.replace (" ", "_", true);
        };

        replaceSpacesWithUnderscore (options.applicationName);
        replaceSpacesWithUnderscore (options.folderName);

        return options;
    }

    /** @returns */
    inline Result createPath (const File& file)
    {
        if (! file.exists())
        {
            const auto result = file.create();
            if (result.failed())
            {
                Logger::writeToLog ("Error creating app storage path: " + file.getFullPathName());
                jassertfalse;
                return result;
            }
        }    

        return Result::ok();
    }

    /** @returns */
    inline File getAppStorageFolderPath()
    {
        return getStorageOptions().getDefaultFile().getParentDirectory();
    }

    /** @returns */
    inline File createAppStorageSubPath (const String& subfolderName)
    {
        const auto file = getAppStorageFolderPath()
                            .getChildFile (subfolderName.toLowerCase());

        if (! file.exists())
        {
            const auto result = file.create();
            if (result.failed())
            {
                Logger::writeToLog ("Error creating app storage path: " + file.getFullPathName());
                jassertfalse;
            }
        }    

        return file;
    }

    /** @returns */
    inline String getLogFileName()
    {
        const auto ct = Time::getCurrentTime();

        String t;

        for (auto v : { ct.getYear(), ct.getMonth(), ct.getDayOfMonth() })
            t << v << "_";

        t << "__";

        for (auto v : { ct.getHours(), ct.getMinutes(), ct.getSeconds(), ct.getMilliseconds() })
            t << v << "_";

        return "logs_" + t + ".txt";
    }

    /** @returns */
    inline File getLogFile()
    {
        return createAppStorageSubPath ("logs/" + getLogFileName());
    }

    /** @returns */
    inline File getLanguagesFolder()
    {
        return createAppStorageSubPath ("languages/");
    }
}
