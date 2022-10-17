class SwitchableTimeParameter : public AudioParameterBool
{
public:
    SwitchableTimeParameter(): AudioParameterBool ("syncswitch",
                                                   "syncswitch",
                                                   false,
                                                   AudioParameterBoolAttributes().withLabel ("Time Sync ").withStringFromValueFunction ([] (bool value, int) -> String {
                                                       if (value > 0)
                                                           return TRANS ("Sync On");
                                                       return TRANS ("Sync Off");
                                                   }))
    {
    }

public:
    void addChildrenToLayout (AudioProcessorValueTreeState::ParameterLayout& layout)
    {
        StringArray options { "1/16", "1/8", "1/4", "1/2", "1", "2", "4", "8", "16" };
        auto beat = std::make_unique<AudioParameterChoice> ("tbeat", "tBeat Division", options, 3);

        NormalisableRange<float> timeRange = { 1.f, 4000.0f };
        auto timep = std::make_unique<NotifiableAudioParameterFloat> ("ydelayTime", "tDelay Time", timeRange, 200.f,
                                                                      true,// isAutomatable
                                                                      "tDelay Time",
                                                                      AudioProcessorParameter::genericParameter,
                                                                      [] (float value, int) -> String {
                                                                          String txt (roundToInt (value));
                                                                          return txt << "ms";
                                                                          ;
                                                                      });

        timeParam = timep.get();

        beatParam = beat.get();

        layout.add (std::move (timep));
        layout.add (std::move (beat));
    }
    float getNextTimeValue()
    {
        return 0.f;
    }

private:
    NotifiableAudioParameterFloat* timeParam = nullptr;
    AudioParameterChoice* beatParam = nullptr;
};
