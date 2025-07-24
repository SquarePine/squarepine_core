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

    /** */
    static std::unique_ptr<Drawable> getMainLogoSVG();
    /** */
    static Image getMainLogoPNG();
    /** */
    static Image getWindowIcon();
    /** */
    static Image getTaskbarIcon();

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
    CREATE_INLINE_CLASS_IDENTIFIER (appSettings)
    CREATE_INLINE_CLASS_IDENTIFIER (appTheme)
    CREATE_INLINE_CLASS_IDENTIFIER (windowBounds)
    CREATE_INLINE_CLASS_IDENTIFIER (maximised)
    CREATE_INLINE_CLASS_IDENTIFIER (audioDeviceSettings)

    //==============================================================================
    const File appSettingsFile;
    const File logFile;

    ValueTree appSettings = ValueTree (appSettingsId);
    Value appTheme, windowBounds, maximised,
          audioDeviceSettings;

    std::unique_ptr<FileLogger> logger;
    std::unique_ptr<LanguageHandler> languageHandler;
    ApplicationCommandManager commandManager;
    KeyPressMappingSet keyPressMappingSet;
    UndoManager undoManager;
    AudioFormatManager audioFormatManager;
    AudioThumbnailCache audioThumbnailCache { 2048 };
    AudioDeviceManager audioDeviceManager;
    std::shared_ptr<FontFamily> lato, defaultFamily;
    std::shared_ptr<ImageFormatManager> imageFormatManager;
    std::shared_ptr<ThreadPool> threadPool;

private:
    //==============================================================================
    void snapRectToCurrentDisplayConfiguration (Rectangle<int>&);

    Value initProperty (const Identifier& id, const var& defaultValue)
    {
        if (! appSettings.hasProperty (id))
            appSettings.setProperty (id, defaultValue, nullptr);

        return appSettings.getPropertyAsValue (id, &undoManager);
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedObjects)
};
