namespace djdawprocessor
{


class VariableBPMProcessor final : public InternalProcessor,
public AudioProcessorParameter::Listener
{
public:
    /** Constructor. */
    VariableBPMProcessor();
    
    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Variabe Tempo Processor"); }
    /** @internal */
    Identifier getIdentifier() const override { return "variableBPMProcessor"; }
    /** @internal */
    bool acceptsMidi() const override { return true; }
    /** @internal */
    bool producesMidi() const override { return true; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    /** @internal */
    void prepareToPlay (double, int) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<float>&, MidiBuffer&) override;
    /** @internal */
    void processBlock (juce::AudioBuffer<double>&, MidiBuffer&) override;
    /** @internal */
    void parameterValueChanged (int, float) override;
    /** @internal */
    void parameterGestureChanged (int, bool) override;
    
private:
    NotifiableAudioParameterFloat* variableBPMParameter = nullptr;
    
    //==============================================================================
    template<typename FloatType>
    struct ChannelDetails final
    {
        void prepare (int numChannels)
        {
            channels.resize (numChannels);
            channels.clearQuick();
            
            tempBuffer.resize (numChannels);
            tempBuffer.clearQuick();
        }
        
        Array<FloatType> channels, tempBuffer;
    };
    
    ChannelDetails<float> floatChannelDetails;
    ChannelDetails<double> doubleChannelDetails;
    
    //==============================================================================
    
    template<typename FloatType>
    void process (juce::AudioBuffer<FloatType>& , ChannelDetails<FloatType>& )
    {
    }
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VariableBPMProcessor)
};


}
