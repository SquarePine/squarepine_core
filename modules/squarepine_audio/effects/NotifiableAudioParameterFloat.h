class NotifiableAudioParameterFloat : public AudioParameterFloat
{
public:
    NotifiableAudioParameterFloat (const String& parameterID,
                                   const String& parameterName,
                                   NormalisableRange<float> normalisableRange,
                                   float defaultValue,
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
                                                        valueFromString) {}
    
    NotifiableAudioParameterFloat   (String parameterID,
                                    String parameterName,
                                    float minValue,
                                    float maxValue,
                                    float defaultValue) :
                                    AudioParameterFloat (parameterID,
                                                         parameterName,
                                                         minValue,
                                                         maxValue,
                                                         defaultValue) {}
protected:
    void valueChanged (float newValue) override
    {
        sendValueChangedMessageToListeners (newValue);
    }
};
