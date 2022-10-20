
InsertProcessor::InsertProcessor (bool bandProcessingActive): performBandProcessing (bandProcessingActive)
{
    auto bypassParam = getBypassParameter();
    if (bypassParam != nullptr)
        bypassParam->setValue (1);
}
InsertProcessor::~InsertProcessor()
{
    for (auto paramCallback: parameterCallbacks)
    {
        if (paramCallback.parameter != nullptr)
            paramCallback.parameter->removeListener (this);
    }
}

void InsertProcessor::setupDefaultParametersAndCallbacks (AudioProcessorValueTreeState::ParameterLayout& layout)
{
    auto fxon = std::make_unique<NotifiableAudioParameterBool> ("fxonoff", "FX On", true, "FX On/Off ", true, [] (bool value, int) -> String {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });

    auto lowFrequencyToggle = std::make_unique<NotifiableAudioParameterBool> ("lowonoff", "Low Frequency Processing", true, "Low Frequency Processing ", true, [] (bool value, int) -> String {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });

    auto midFrequencyToggle = std::make_unique<NotifiableAudioParameterBool> ("midonoff", "Mid Frequency Processing", true, "Mid Frequency Processing ", true, [] (bool value, int) -> String {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });

    auto highFrequencyToggle = std::make_unique<NotifiableAudioParameterBool> ("highonoff", "High Frequency Processing", true, "High Frequency Processing ", true, [] (bool value, int) -> String {
        if (value > 0)
            return TRANS ("On");
        return TRANS ("Off");
        ;
    });

    fxOnParam = fxon.get();
    layout.add (std::move (fxon));
    addParameterWithCallback (fxOnParam, [&] (float val) {
        setBypass (val > 0);
    });

    lowFrequencyToggleParam = lowFrequencyToggle.get();
    midFrequencyToggleParam = midFrequencyToggle.get();
    highFrequencyToggleParam = highFrequencyToggle.get();

    if (performBandProcessing)
    {
        layout.add (std::move (lowFrequencyToggle));
        layout.add (std::move (midFrequencyToggle));
        layout.add (std::move (highFrequencyToggle));
        addParameterWithCallback (lowFrequencyToggleParam, [&] (float) {
        });
        addParameterWithCallback (midFrequencyToggleParam, [&] (float) {
        });
        addParameterWithCallback (highFrequencyToggleParam, [&] (float) {
        });
    }
}

void InsertProcessor::parameterValueChanged (int paramIndex, float val)
{
    const ScopedLock sl (getCallbackLock());
    //  parameterCallbacks[paramIndex].callback(val);
    auto parameterCallback = std::find (parameterCallbacks.begin(), parameterCallbacks.end(), paramIndex);
    if (parameterCallback != parameterCallbacks.end())
        parameterCallback->callback (val);
}

void InsertProcessor::prepareToPlay (double Fs, int bufferSize)
{
    setRateAndBufferSizeDetails (Fs, bufferSize);
}
void InsertProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    //This should split the signal into bands if required
    //Then the processaudioblock should be called to perform the processing

    auto bypassParam = getBypassParameter();
    if (bypassParam != nullptr)
        if (bypassParam->getValue() == 0)
        {
            return;
        };
    processAudioBlock (buffer, midi);
}

void InsertProcessor::addParameterWithCallback (AudioProcessorParameter* rawParameter, std::function<void (float&)> callback)
{
    jassert (rawParameter != nullptr);
    rawParameter->addListener (this);
    addParameterCallback ((int) getNumberOfParameters() + 1, rawParameter, callback);
}


void InsertProcessor::addParameterCallback (const int parameterIndex, AudioProcessorParameter* rawParameter, std::function<void (float&)> parameterCallback)
{
    //this may need to ensure that you are not adding one with an existing parameter;
    auto existingCallback = std::find (parameterCallbacks.begin(), parameterCallbacks.end(), parameterIndex);
    //Oops it looks like you are adding a parameter with an index that is already taken.
    jassert (existingCallback == parameterCallbacks.end());
    if(existingCallback == parameterCallbacks.end()) parameterCallbacks.push_back (ParameterCallback (parameterIndex, rawParameter, parameterCallback));
}
//***********
//***********



void InsertProcessor::linkParameters(ParameterLinker paramLink)
{
    linkedParameters.push_back(paramLink);
}
bool InsertProcessor::isParameterLinked(RangedAudioParameter* param)
{
    jassert(param != nullptr);
    for(auto paramLink : linkedParameters)
    {
        if(paramLink.contains(param))return true;
    }
    
    return false;
}
bool InsertProcessor::isPrimaryLinkedParameter(RangedAudioParameter* param)
{
    jassert(param != nullptr);

    for(auto paramLink : linkedParameters)
    {
        if(param == paramLink.getPrimaryParam())return true;
    }
    return false;
}
InsertProcessor::ParameterLinker* InsertProcessor::getLinkedParametersFor(RangedAudioParameter* param)
{
    jassert(param != nullptr);

    for(unsigned long i = 0 ; i < linkedParameters.size();++i)
    {
        if(linkedParameters[i].contains(param))return &linkedParameters[i];
    }
    return nullptr;
}
