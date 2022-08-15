/// This placeholder class with No DSP.  It's purpose is to provide an appropriate parameter interface for recording useful information..

class DubEchoProcessor final : public InternalProcessor,
                             public AudioProcessorParameter::Listener
{
public:
    //Constructor with ID
    DubEchoProcessor (int idNum = 1);
    ~DubEchoProcessor()override;

    //============================================================================== Audio processing
    void prepareToPlay (double Fs, int bufferSize) override;
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&) override;
    //============================================================================== House keeping
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override;
    //============================================================================== Parameter callbacks
    void parameterValueChanged (int paramNum, float value) override;
    void parameterGestureChanged (int, bool) override{}

private:
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    NotifiableAudioParameterFloat* wetDryParam = nullptr;
    NotifiableAudioParameterFloat* timeParam = nullptr;
    NotifiableAudioParameterFloat* echoColourParam = nullptr;
    NotifiableAudioParameterFloat* feedbackParam = nullptr;
    NotifiableAudioParameterFloat* fxOnParam = nullptr;

    
    int idNumber = 1;

  
};

