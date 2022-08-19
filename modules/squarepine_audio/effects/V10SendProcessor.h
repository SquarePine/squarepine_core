///This is a wrapper class specifically for the effect sends on the V10
///This class should do no processing but the extra dry wet parameter should be shown when created in performance with a V10
class V10SendProcessor : public InternalProcessor,
                         public AudioProcessorParameter::Listener
{
public:
    V10SendProcessor() {}
    ~V10SendProcessor() {}

    void appendExtraParams (AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        NormalisableRange<float> wetDryRange = { 0.f, 1.f };
        auto extendedwetdry = std::make_unique<NotifiableAudioParameterFloat> ("V10Dry/Wet", "V10 Dry/Wet", wetDryRange, 0.5f,
                                                                               true,// isAutomatable
                                                                               "V10 Dry/Wet",
                                                                               AudioProcessorParameter::genericParameter,
                                                                               [] (float value, int) -> String {
                                                                                   int percentage = roundToInt (value * 100);
                                                                                   String txt (percentage);
                                                                                   return txt << "%";
                                                                               });

        extendedWetDryParam = extendedwetdry.get();
        layout.add (std::move (extendedwetdry));
    }

    void enableExtendedControl (bool state)
    {
        extendedWetDryParam->setAutomatable (state);
    }

private:
    NotifiableAudioParameterFloat* extendedWetDryParam;
};
