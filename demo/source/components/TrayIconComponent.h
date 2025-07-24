#if SQUAREPINE_IS_DESKTOP

/** This is just a generic and simplistic tray icon component,
    instantiated on desktop systems only (which is the only place it's supported by JUCE).

    @see MainWindow, juce::SystemTrayIconComponent
*/
class TrayIconComponent final : public SystemTrayIconComponent
{
public:
    TrayIconComponent (ApplicationCommandManager& cm) :
        commandManager (cm)
    {
        const auto image = SharedObjects::getTaskbarIcon();
        setIconImage (image, image);
    }

    //==============================================================================
    void mouseDown (const MouseEvent& e) override
    {
        lastEventMods = e.mods;
    }

    void mouseUp (const MouseEvent&) override
    {
        auto& desktop = Desktop::getInstance();

        for (int i = desktop.getNumComponents(); --i >= 0;)
            desktop.getComponent (i)->toFront (true);

        // NB: by this point, the mouse buttons are... well, up/inactive.
        if (lastEventMods.isPopupMenu()
            && commandManager.getNumCommands() > 0)
        {
            PopupMenu pm;

            pm.addCommandItem (&commandManager, WorkstationIds::preferences);
            pm.addSeparator();
            pm.addCommandItem (&commandManager, StandardApplicationCommandIDs::quit);

            pm.showMenuAsync ({});
        }

        lastEventMods = {};
    }

private:
    //==============================================================================
    ApplicationCommandManager& commandManager;
    ModifierKeys lastEventMods;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrayIconComponent)
};

#endif // SQUAREPINE_IS_DESKTOP
