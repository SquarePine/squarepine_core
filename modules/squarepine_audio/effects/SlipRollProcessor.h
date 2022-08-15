/// This placeholder class with No DSP.  It's purpose is to provide an appropriate parameter interface for recording useful information..

class SlipRollProcessor final : public BandProcessor
{
public:
    //Constructor with ID
    SlipRollProcessor (int idNum = 1);
    ~SlipRollProcessor()override;

    //============================================================================== Audio processing
    void prepareToPlay (double Fs, int bufferSize) override;
    void processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&) override;
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
    NotifiableAudioParameterFloat* beatParam = nullptr;
    NotifiableAudioParameterFloat* timeParam = nullptr;
    NotifiableAudioParameterFloat* wetDryParam = nullptr;
    NotifiableAudioParameterFloat* xPadParam = nullptr;
    NotifiableAudioParameterFloat* onOffParam = nullptr;
    NotifiableAudioParameterFloat* fxOnParam = nullptr;
    
    int idNumber = 1;

  
};

