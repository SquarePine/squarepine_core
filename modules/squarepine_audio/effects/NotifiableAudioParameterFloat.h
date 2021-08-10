class NotifiableAudioParameterFloat : public AudioParameterFloat
{
public:
    NotifiableAudioParameterFloat (const String& parameterID,
                                   const String& parameterName,
                                   NormalisableRange<float> normalisableRange,
                                   float defaultValue,
                                   bool automate,
                                   const String& parameterLabel = String(),
                                   Category parameterCategory = AudioProcessorParameter::genericParameter,
                                   std::function<String (float value, int maximumStringLength)> stringFromValue = nullptr,
                                   std::function<float (const String& text)> valueFromString = nullptr) :
                                   AudioParameterFloat (parameterID,
                                                        parameterName,
                                                        normalisableRange,
                                                        defaultValue,
                                                        parameterLabel,
                                                        parameterCategory,
                                                        stringFromValue,
                                                        valueFromString),
                                    automatable (automate) {}
    
    
    NotifiableAudioParameterFloat  (String parameterID,
                                    String parameterName,
                                    float minValue,
                                    float maxValue,
                                    float defaultValue,
                                    bool automate) :
                                    AudioParameterFloat (parameterID,
                                                         parameterName,
                                                         minValue,
                                                         maxValue,
                                                         defaultValue),
                                    automatable (automate) {}
    
    bool isAutomatable() const override { return automatable; }
    
protected:
    void valueChanged (float newValue) override
    {
        sendValueChangedMessageToListeners (newValue);
    }
private:
    const bool automatable;
};
