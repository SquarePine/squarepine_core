namespace djdawprocessor
{

class ShortDelayProcessor final : public V10SendProcessor
{
public:
    //Constructor with ID
    ShortDelayProcessor (int idNum = 1);
    ~ShortDelayProcessor()override;
    
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
    NotifiableAudioParameterFloat* colourParam = nullptr;
    NotifiableAudioParameterFloat* feedbackParam = nullptr;
    AudioParameterBool* fxOnParam = nullptr;
    
    
    int idNumber = 1;
    
    
};

}
