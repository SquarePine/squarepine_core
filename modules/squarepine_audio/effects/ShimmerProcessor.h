/// This placeholder class with No DSP.  It's purpose is to provide an appropriate parameter interface for recording useful information..

class ShimmerProcessor final : public BandProcessor
{
public:
    //Constructor with ID
    ShimmerProcessor (int idNum = 1);
    ~ShimmerProcessor()override;

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
    NotifiableAudioParameterFloat* reverbAmountParam = nullptr;
    NotifiableAudioParameterFloat* timeParam = nullptr;
    NotifiableAudioParameterFloat* wetDryParam = nullptr;
    NotifiableAudioParameterFloat* xPadParam = nullptr;
    NotifiableAudioParameterFloat* fxOnParam = nullptr;

    
    int idNumber = 1;

  
};

