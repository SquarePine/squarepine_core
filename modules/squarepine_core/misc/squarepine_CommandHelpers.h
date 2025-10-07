/**
*/
class AsyncCommandRetrier final : private Timer
{
public:
    /**
    */
    AsyncCommandRetrier (ApplicationCommandManager&,
                         const ApplicationCommandTarget::InvocationInfo&);

    /** @internal */
    void timerCallback() override;

private:
    ApplicationCommandManager& commandManager;
    ApplicationCommandTarget::InvocationInfo info;

    JUCE_DECLARE_NON_COPYABLE (AsyncCommandRetrier)
};

//============================================================================
/**
*/
bool reinvokeCommandAfterCancellingModalComps (ApplicationCommandManager&,
                                               const ApplicationCommandTarget::InvocationInfo&);

/**
*/
void addShortcut (Array<KeyPress>& keyPresses, const KeyPress&);

/**
*/
void addShortcut (ApplicationCommandInfo& result, const KeyPress&);

/**
*/
void addShortcut (ApplicationCommandInfo& result, const String& description);

/**
*/
void addShortcut (ApplicationCommandInfo& result, int keycode);
