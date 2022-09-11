#pragma once

/** A set of general-purpose media application command IDs.

    Because these commands are likely to be used in most apps, they're defined
    here to help different apps to use the same numeric values for them.

    @see ApplicationCommandInfo, ApplicationCommandManager,
         ApplicationCommandTarget, KeyPressMappingSet
*/
namespace MediaApplicationCommandIDs
{
    enum
    {
        play = 0x1101,
        stop,
        pause,
        rewind,
        fastForward,
        toggleLoop,
        masterGainUp,
        masterGainDown,
        masterGainMute
    };
}

//==============================================================================
/** A set of general-purpose audio workstation application command IDs.

    Because these commands are likely to be used in most apps, they're defined
    here to help different apps to use the same numeric values for them.

    @see ApplicationCommandInfo, ApplicationCommandManager,
         ApplicationCommandTarget, KeyPressMappingSet
*/
namespace WorkstationIds
{
    enum
    {
        preferences = 0x1200,
        newProject,
        saveProject,
        saveProjectAs,
        loadProject,
        exportProject,
        recentProjects,
        import,
        clear,
        sortByName,
        sortByType,
        openLogsDirectory,
        openMyVibesFolder,
        openVibeExportFolder
    };
}

//==============================================================================
/** */
namespace GeneralIds
{
    enum
    {
        showHelp = 0x1301,
        showWebsite,
        showAbout
    };
}

//============================================================================
namespace CommandCategories
{
    static const char* const general    = NEEDS_TRANS ("General");
    static const char* const editing    = NEEDS_TRANS ("Editing");
    static const char* const view       = NEEDS_TRANS ("View");
    static const char* const windows    = NEEDS_TRANS ("Windows");
}

