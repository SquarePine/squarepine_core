/** This controls and stores an assortment of application related data.

    It's accessible anywhere within the UI code. Life's too short to pass
    independant objects around for these kinds of things.

    So far, this controls, saves, and restores:
    - File logging
    - Window settings (position, size, fullscreen or not)
    - Translation files
    - Saving/restoring the audio device settings
    - An UndoManager to manage a stack of undoable actions (which is used to display the actions in debug in the menu bar)
    - An ApplicationCommandManager to manage all related commands for a project and app
*/
class SharedObjects final : public Value::Listener
{
public:
    /** */
    SharedObjects();

    /** */
    ~SharedObjects() override;

    //==============================================================================
    /** @returns true if the app is set to a fullscreen mode.

        Only applies to desktop.
    */
    bool isFullscreen() const;

    /** */
    void toggleFullscreen();

    //==============================================================================
    /** */
    static Rectangle<int> getDefaultWindowDimensions() noexcept   { return { 1024, 768 }; } // { return resolution::createXGA(); }
    /** */
    static Rectangle<int> getMinimumWindowDimensions() noexcept   { return { 400, 400 }; }

    /** */
    void saveWindowDimensions (const DocumentWindow&);

    /** */
    void restoreWindowDimensions (DocumentWindow&);

    //==============================================================================
    /** */
    void saveRecentFiles (const RecentlyOpenedFilesList&);

    /** */
    void restoreRecentFiles (RecentlyOpenedFilesList&);

    //==============================================================================
    /** */
    Result saveAudioDeviceSetup();

    /** */
    Result reloadAudioDeviceManagerFromSettings();

    //==============================================================================
    void clear (bool notifyCommandStatus);

    //==============================================================================
    /** */
    static void populate (AudioFormatManager&);

    //==============================================================================
    /** @internal */
    void valueChanged (Value&) override;

    //==============================================================================
    CREATE_INLINE_CLASS_IDENTIFIER (windowSettings)
    CREATE_INLINE_CLASS_IDENTIFIER (bounds)
    CREATE_INLINE_CLASS_IDENTIFIER (maximised)
    CREATE_INLINE_CLASS_IDENTIFIER (audioDeviceSettings)
    CREATE_INLINE_CLASS_IDENTIFIER (recentFiles)

    //==============================================================================
    const File logFile;
    std::unique_ptr<FileLogger> logger;
    mutable ApplicationProperties applicationProperties;
    std::unique_ptr<LanguageHandler> languageHandler;
    ApplicationCommandManager commandManager;
    KeyPressMappingSet keyPressMappingSet;
    UndoManager undoManager;

    AudioFormatManager audioFormatManager;
    AudioThumbnailCache audioThumbnailCache { 2048 };
    AudioDeviceManager audioDeviceManager;

    Value fullscreen;

    FontFamily lato { "Lato" };
    FontFamily* defaultFamily = &lato;

private:
    //==============================================================================
    void snapRectToCurrentDisplayConfiguration (Rectangle<int>&);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedObjects)
};
