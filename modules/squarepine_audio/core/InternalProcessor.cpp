//==============================================================================
class InternalProcessor::BypassParameter final : public AudioParameterBool
{
public:
    BypassParameter() :
        AudioParameterBool (ParameterID (InternalProcessor::bypassId.toString(), 1),
                            NEEDS_TRANS ("Bypass"), false)
    {
    }

    String getText (float v, int maximumStringLength) const override
    {
        return (approximatelyEqual (v, 0.0f) ? TRANS ("Active") : TRANS ("Bypassed"))
                .substring (0, maximumStringLength);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BypassParameter)
};

//==============================================================================
InternalProcessor::ScopedBypass::ScopedBypass (InternalProcessor& ip) :
    internalProcessor (ip),
    wasBypassed (ip.isBypassed())
{
    if (! wasBypassed)
        internalProcessor.setBypassed (true);
}

InternalProcessor::ScopedBypass::~ScopedBypass()
{
    if (! wasBypassed)
        internalProcessor.setBypassed (false);
}

//==============================================================================
InternalProcessor::InternalProcessor (bool applyDefaultBypassParam)
{
    if (applyDefaultBypassParam)
    {
        bypassParameter = createBypassParameter().release();
        AudioProcessor::addParameter (bypassParameter);
    }

    resetBuses (*this, 2, 2);
    setRateAndBufferSizeDetails (44100.0, 256);
}

//==============================================================================
std::unique_ptr<AudioParameterBool> InternalProcessor::createBypassParameter() const
{
    return std::make_unique<BypassParameter>();
}

AudioProcessorValueTreeState::ParameterLayout InternalProcessor::createDefaultParameterLayout (bool addBypassParam)
{
    AudioProcessorValueTreeState::ParameterLayout layout;

    if (addBypassParam)
    {
        auto bp = createBypassParameter();
        bypassParameter = bp.get();
        layout.add (std::move (bp));
    }

    return layout;
}

void InternalProcessor::resetAPVTSWithLayout (AudioProcessorValueTreeState::ParameterLayout&& layout,
                                              UndoManager* undoManagerToUse)
{
    apvts.reset (new AudioProcessorValueTreeState (*this, undoManagerToUse,
                                                   "parameters", std::move (layout)));
}

//==============================================================================
ValueTree InternalProcessor::getState() const
{
    JUCE_ASSERT_MESSAGE_THREAD

    return hasAPVTS()
            ? apvts->state
            : ValueTree();
}

Value InternalProcessor::getPropertyAsValue (const Identifier& id, UndoManager* um, bool b)
{
    return getState().getPropertyAsValue (id, um, b);
}

const var& InternalProcessor::getProperty (const Identifier& id) const
{
    return getState().getProperty (id);
}

var InternalProcessor::getProperty (const Identifier& id, const var& d) const
{
    return getState().getProperty (id, d);
}

const var* InternalProcessor::getPropertyPointer (const Identifier& id) const
{
    return getState().getPropertyPointer (id);
}

InternalProcessor& InternalProcessor::setProperty (const Identifier& id, const var& v, UndoManager* um)
{
    getState().setProperty (id, v, um);
    return *this;
}

bool InternalProcessor::hasProperty (const Identifier& id) const
{
    return getState().hasProperty (id);
}

void InternalProcessor::removeProperty (const Identifier& id, UndoManager* um)
{
    getState().removeProperty (id, um);
}

//==============================================================================
void InternalProcessor::setBypassed (const bool shouldBeBypassed)
{
    if (bypassParameter != nullptr)
        bypassParameter->operator= (shouldBeBypassed);
}

bool InternalProcessor::isBypassed() const noexcept
{
    return bypassParameter != nullptr
            ? bypassParameter->get()
            : false;
}

//==============================================================================
void InternalProcessor::prepareToPlay (const double sampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);
}

//==============================================================================
void InternalProcessor::fillInPluginDescription (PluginDescription& description) const
{
    description.name                = getName().trim();
    description.descriptiveName     = description.name;
    description.pluginFormatName    = getInternalProcessorTypeName();
    description.category            = isInstrument() ? TRANS ("Synth") : TRANS ("Effect");
    description.manufacturerName    = {};
    description.version             = getVersion();
    description.fileOrIdentifier    = getIdentifier().toString();
    description.lastFileModTime     = Time::getCurrentTime();
    description.uniqueId            = description.name.hashCode();
    description.isInstrument        = isInstrument();
    description.numInputChannels    = getTotalNumInputChannels();
    description.numOutputChannels   = getTotalNumOutputChannels();
}

//==============================================================================
void InternalProcessor::getStateInformation (MemoryBlock& destData)
{
    ValueTree valueTree (getIdentifier());

    if (apvts != nullptr)
    {
        valueTree = apvts->copyState();
    }
    else
    {
        for (auto* param : getParameters())
            if (auto* const p = dynamic_cast<AudioProcessorParameterWithID*> (param))
                valueTree.setProperty (Identifier (p->paramID), p->getValue(), nullptr);
    }

    if (auto state = valueTree.createXml())
        copyXmlToBinary (*state, destData);
}

void InternalProcessor::setStateInformation (const void* data, const int sizeInBytes)
{
    if (auto state = getXmlFromBinary (data, sizeInBytes))
    {
        const auto valueTree = ValueTree::fromXml (*state);

        if (apvts != nullptr)
        {
            apvts->replaceState (valueTree);
        }
        else
        {
            if (valueTree.hasType (getIdentifier()))
                for (int i = 0; i < valueTree.getNumProperties(); ++i)
                    for (auto* param : getParameters())
                        if (auto* const p = dynamic_cast<AudioProcessorParameterWithID*> (param))
                            if (const auto propName = valueTree.getPropertyName (i); Identifier (p->paramID) == propName)
                                p->setValue (valueTree.getProperty (propName));
        }
    }

    updateHostDisplay();
}
