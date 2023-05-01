EffectProcessor::EffectProcessor (AudioPluginPtr api, const PluginDescription& pd) :
    plugin (std::move (api)),
    description (pd)
{
    SQUAREPINE_CRASH_TRACER

    setName ({});
    setMixLevel (1.0f);
    setBypassed (false);

    if (plugin != nullptr)
    {
        setName (plugin->getName());

        // Capture the plugin's state at start-up in case the developers
        // haven't implemented the getParameterDefaultValue interface
        MemoryBlock defaultState;
        plugin->getStateInformation (defaultState);
        setDefaultState (defaultState);
    }

    state.addListener (this);
}

EffectProcessor::~EffectProcessor()
{
    state.removeListener (this);
}

//==============================================================================
bool EffectProcessor::canBeProcessed() const noexcept
{
    return ! isMissing() && ! isBypassed();
}

String EffectProcessor::getName() const                                 { return state[nameId].toString(); }
void EffectProcessor::setName (const String& n, UndoManager* um)        { state.setProperty (nameId, n, um); }
Value EffectProcessor::getNameValueObject (UndoManager* um, bool b)     { return state.getPropertyAsValue (nameId, um, b); }

float EffectProcessor::getMixLevel() const                              { return static_cast<float> (state[mixId]); }
void EffectProcessor::setMixLevel (float m, UndoManager* um)            { state.setProperty (mixId, inclusiveClamp (m, 0.0f, 1.0f), um); }
Value EffectProcessor::getMixValueObject (UndoManager* um, bool b)      { return state.getPropertyAsValue (mixId, um, b); }

bool EffectProcessor::isBypassed() const                                { return static_cast<bool> (state[bypassedId]); }
void EffectProcessor::setBypassed (bool b, UndoManager* um)             { state.setProperty (bypassedId, b, um); }
Value EffectProcessor::getBypassValueObject (UndoManager* um, bool b)   { return state.getPropertyAsValue (bypassedId, um, b); }

void EffectProcessor::valueTreePropertyChanged (ValueTree&, const Identifier& id)
{
    if (id == mixId)
        mixLevel.setTargetValue (static_cast<float> (state[mixId]));
    else if (id == bypassedId)
        bypassed = static_cast<bool> (state[bypassedId]);
}

//==============================================================================
void EffectProcessor::setDefaultState (const MemoryBlock& data)
{
    state.setProperty (defaultStateId, data, nullptr);
}

void EffectProcessor::setLastState (const MemoryBlock& data)
{
    state.setProperty (lastStateId, data, nullptr);
}

bool EffectProcessor::reloadFromStateIfValid()
{
    if (isMissing())
        return false;

    const auto& lastState = state[lastStateId];
    if (auto* data = lastState.getBinaryData())
        plugin->setStateInformation (data->getData(), (int) data->getSize());

    setLastState ({});
    return true;
}
