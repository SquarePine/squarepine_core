/**
*/
class AsyncCommandRetrier final : private Timer
{
public:
    /**
    */
    AsyncCommandRetrier (ApplicationCommandManager& acm,
                         const ApplicationCommandTarget::InvocationInfo& inf);

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
bool reinvokeCommandAfterCancellingModalComps (ApplicationCommandManager& acm,
                                               const ApplicationCommandTarget::InvocationInfo& info);

/**
*/
void addShortcut (Array<KeyPress>& keyPresses, const KeyPress& keyPress);

/**
*/
void addShortcut (ApplicationCommandInfo& result, const KeyPress& keyPress);

/**
*/
void addShortcut (ApplicationCommandInfo& result, const String& description);

/**
*/
void addShortcut (ApplicationCommandInfo& result, int keycode);
