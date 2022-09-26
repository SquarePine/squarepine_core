class NotifiableAudioParameterBool : public AudioParameterBool
{
public:
    NotifiableAudioParameterBool (const String& parameterID,
                                  const String& parameterName,
                                  bool defaultValue,
                                  const String& parameterLabel,
                                  bool automate,
                                  std::function<String (float value, int maximumStringLength)> stringFromValue = nullptr): AudioParameterBool (parameterID,
                                                                                                                                               parameterName,
                                                                                                                                               defaultValue,
                                                                                                                                               parameterLabel,
                                                                                                                                               stringFromValue),
                                                                                                                           automatable (automate)
    {
    }

    bool isAutomatable() const override { return automatable; }
    void setAutomatable (const bool state) { automatable = state; }

protected:
    void valueChanged (bool newValue) override
    {
        sendValueChangedMessageToListeners (newValue);
    }

private:
    bool automatable;
};
