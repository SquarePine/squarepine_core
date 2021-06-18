//==============================================================================
class InternalProcessor::BypassParameter final : public AudioParameterBool
{
public:
    BypassParameter() :
        AudioParameterBool ("bypassId", TRANS ("Bypass"), false)
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
        internalProcessor.setBypass (true);
}

InternalProcessor::ScopedBypass::~ScopedBypass()
{
    if (! wasBypassed)
        internalProcessor.setBypass (false);
}

//==============================================================================
InternalProcessor::InternalProcessor() :
    bypassParameter (new BypassParameter())
{
    addParameter (bypassParameter);

    resetBuses (*this, 2, 2);
    setRateAndBufferSizeDetails (44100.0, 256);
}

//==============================================================================
void InternalProcessor::setBypass (const bool shouldBeBypassed)
{
    bypassParameter->AudioParameterBool::operator= (shouldBeBypassed);
}

bool InternalProcessor::isBypassed() const noexcept
{
    return bypassParameter->get();
}

//==============================================================================
void InternalProcessor::prepareToPlay (const double sampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, estimatedSamplesPerBlock);
}

//==============================================================================
AudioProcessorParameter* InternalProcessor::getBypassParameter() const  { return bypassParameter; }
double InternalProcessor::getTailLengthSeconds() const                  { return 0.0; }
bool InternalProcessor::hasEditor() const                               { return false; }
AudioProcessorEditor* InternalProcessor::createEditor()                 { return nullptr; }
void InternalProcessor::releaseResources()                              { }
bool InternalProcessor::acceptsMidi() const                             { return false; }
bool InternalProcessor::producesMidi() const                            { return false; }
int InternalProcessor::getNumPrograms()                                 { return 1; }
int InternalProcessor::getCurrentProgram()                              { return 0; }
void InternalProcessor::setCurrentProgram (int)                         { }
const String InternalProcessor::getProgramName (int)                    { return TRANS ("Default"); }
void InternalProcessor::changeProgramName (int, const String&)          { }
bool InternalProcessor::isInstrument() const                            { return false; }
String InternalProcessor::getVersion() const                            { return "1.0"; }

//==============================================================================
void InternalProcessor::fillInPluginDescription (PluginDescription& description) const
{
    description.name                = getName().trim();
    description.descriptiveName     = description.name;
    description.pluginFormatName    = getInternalProcessorTypeName();
    description.category            = isInstrument() ? TRANS ("Synth") : TRANS ("Effect");
    description.manufacturerName    = "";
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

    {
        ScopedLock sl (getCallbackLock());

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
        ValueTree valueTree (ValueTree::fromXml (*state));

        if (valueTree.getType() == getIdentifier())
        {
            ScopedLock sl (getCallbackLock());

            for (int i = 0; i < valueTree.getNumProperties(); ++i)
                for (auto* param : getParameters())
                    if (auto* const p = dynamic_cast<AudioProcessorParameterWithID*> (param))
                        if (Identifier (p->paramID) == valueTree.getPropertyName (i))
                            p->setValue (valueTree.getProperty (valueTree.getPropertyName (i)));
        }
    }

    updateHostDisplay();
}
