///This is a handy class that is used to provide an assortment of core insert functionality
///This includes the F/X Active control, band processing (if required) and syncable controls
///The class provides a mechanism for obtaining syncable parameters
///The class provides a mechanism for adding parameters and registering callbacks
namespace djdawprocessor
{

class InsertProcessor : public InternalProcessor,
                        public AudioProcessorParameter::Listener
{
    struct ParameterCallback
    {
        int index;
        AudioProcessorParameter* parameter;
        std::function<void (float&)> callback;
        ParameterCallback (int parameterIndex, AudioProcessorParameter* rawParameter, std::function<void (float&)> parameterCallback)
            : index (parameterIndex),
              parameter (rawParameter),
              callback (parameterCallback)
        {
        }
        bool operator== (const int& parameterIndex)
        {
            return index == parameterIndex;
        }
    };

    enum DefaultParamIndexes
    {
        FX_BYPASS = 0,
        FX_ACTIVE,
        LOW_FREQUENCY_PROCESSING,
        MID_FREQUENCY_PROCESSING,
        HIGH_FREQUENCY_PROCESSING,
        NUM_PARAMS
    };

    int getNumberOfDefaultParameters()
    {
        return performBandProcessing == true ? LOW_FREQUENCY_PROCESSING : NUM_PARAMS;
    }

    unsigned long int getNumberOfParameters()
    {
        return parameterCallbacks.size();
    }
    void addParameterCallback (const int parameterIndex, AudioProcessorParameter* rawParameter, std::function<void (float&)> parameterCallback);

    //***************
    //***************
public:
    class ParameterLinker
    {
        std::vector<RangedAudioParameter*> linkedParams;
        RangedAudioParameter* primaryParam;
    public:
        ParameterLinker()
        {
        }
        void addParameterToLink (RangedAudioParameter* param)
        {
            jassert (param != nullptr);

            auto it = std::find (linkedParams.begin(), linkedParams.end(), param);
            if (it == linkedParams.end())
                linkedParams.push_back (param);
        }
        void setPrimaryParameter (RangedAudioParameter* param)
        {
            jassert (param != nullptr);

            primaryParam = param;
        }
        bool contains (RangedAudioParameter* param)
        {
            jassert (param != nullptr);

            auto it = std::find (linkedParams.begin(), linkedParams.end(), param);
            if (it != linkedParams.end())
                return true;

            if (param == primaryParam)
                return true;

            return false;
        }
        RangedAudioParameter* getPrimaryParam()
        {
            return primaryParam;
        }
    };
public:
    InsertProcessor (bool bandProcessingActive);
    ~InsertProcessor() override;

    void prepareToPlay (double Fs, int bufferSize) override;
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midi) override;

    //The abstract function wherein inhereted classes should perform their DSP
    virtual void processAudioBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midi) = 0;

    void setupDefaultParametersAndCallbacks (AudioProcessorValueTreeState::ParameterLayout& layout);

    void addParameterWithCallback (AudioProcessorParameter* rawParameter, std::function<void (float&)> callback);

    virtual void parameterValueChanged (int paramNum, float value) override;

    void linkParameters (ParameterLinker paramLink);
    bool isParameterLinked (RangedAudioParameter* param);
    bool isPrimaryLinkedParameter (RangedAudioParameter* param);
    ParameterLinker* getLinkedParametersFor (RangedAudioParameter* param);
private:
    bool performBandProcessing = false;
    NotifiableAudioParameterBool *lowFrequencyToggleParam, *midFrequencyToggleParam, *highFrequencyToggleParam, *fxOnParam;

    typedef std::vector<ParameterCallback> ParameterCallbackVector;
    ParameterCallbackVector parameterCallbacks;

    std::vector<ParameterLinker> linkedParameters;
};

}
