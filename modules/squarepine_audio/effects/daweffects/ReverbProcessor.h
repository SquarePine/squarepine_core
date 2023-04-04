namespace djdawprocessor
{

class ReverbProcessor final : public BandProcessor
{
public:
    //Constructor with ID
    ReverbProcessor (int idNum = 1);
    ~ReverbProcessor() override;

    //============================================================================== Audio processing
    void prepareToPlay (double Fs, int bufferSize) override;
    void processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&) override;
    /** @internal */
    void releaseResources() override;
    //============================================================================== House keeping
    const String getName() const override;
    /** @internal */
    Identifier getIdentifier() const override;
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override;
    //============================================================================== Parameter callbacks
    void parameterValueChanged (int paramNum, float value) override;
    void parameterGestureChanged (int, bool) override {}
private:
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    NotifiableAudioParameterFloat* reverbAmountParam = nullptr;
    NotifiableAudioParameterFloat* timeParam = nullptr;
    NotifiableAudioParameterFloat* wetDryParam = nullptr;
    NotifiableAudioParameterFloat* xPadParam = nullptr;
    AudioParameterBool* fxOnParam = nullptr;
    //Using the Juce reverb
    Reverb reverb;
    void updateReverbParams();

    int idNumber = 1;
};

}
