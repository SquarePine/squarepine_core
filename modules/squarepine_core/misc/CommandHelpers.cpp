//============================================================================
AsyncCommandRetrier::AsyncCommandRetrier (ApplicationCommandManager& acm,
                                          const ApplicationCommandTarget::InvocationInfo& inf) :
    commandManager (acm),
    info (inf)
{
    info.originatingComponent = nullptr;
    startTimer (500);
}

void AsyncCommandRetrier::timerCallback()
{
    stopTimer();
    commandManager.invoke (info, true);
    delete this;
}

//============================================================================
bool reinvokeCommandAfterCancellingModalComps (ApplicationCommandManager& acm,
                                               const ApplicationCommandTarget::InvocationInfo& info)
{
    if (ModalComponentManager::getInstance()->cancelAllModalComponents())
    {
        new AsyncCommandRetrier (acm, info);
        return true;
    }

    return false;
}

void addShortcut (Array<KeyPress>& keyPresses, const KeyPress& keyPress)
{
    keyPresses.addIfNotAlreadyThere (keyPress);
}

void addShortcut (ApplicationCommandInfo& result, const KeyPress& keyPress)
{
    addShortcut (result.defaultKeypresses, keyPress);
}

void addShortcut (ApplicationCommandInfo& result, const String& description)
{
    addShortcut (result, KeyPress::createFromDescription (description));
}

void addShortcut (ApplicationCommandInfo& result, int keycode)
{
    addShortcut (result.defaultKeypresses, KeyPress (keycode));
}
