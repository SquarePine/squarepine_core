
// This model is based on the Virtual Analog analysis
// by Will Pirkle: https://github.com/ddiakopoulos/MoogLadders/blob/master/src/OberheimVariationModel.h


class SEMFilter final : public InternalProcessor,
                           public AudioProcessorParameter::Listener
{
public:
	
	SEMFilter()
    {
        reset();
        
        NormalisableRange<float> freqRange = { -1.f, 1.f};
        auto normFreq = std::make_unique<NotifiableAudioParameterFloat> ("freqSEM","Frequency",
                                                                   freqRange,
                                                                   0.0f,
                                                                   true, // isAutomatable
                                                                   "Cut-off",
                                                                     AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
                                                                        if (approximatelyEqual (value,0.0f))
                                                                            return "BYP";
                                                                            
                                                                        if (value < 0.0f)
                                                                        {
                                                                            float posFreq = value + 1.f;
                                                                            float freqHz = 2.f * std::powf(10.f,3.f*posFreq + 1.f);
                                                                            return String(freqHz,0);
                                                                        }
                                                                        else
                                                                        {
                                                                            float freqHz = 2.f * std::powf(10.f,3.f*value + 1.f);
                                                                            return String(freqHz,0);
                                                                        }
                                                                   });
        
        NormalisableRange<float> qRange = { 0.1f, 10.f};
        auto res = std::make_unique<NotifiableAudioParameterFloat> ("resSEM","resonance",
                                                                   qRange,
                                                                   0.7071f,
                                                                   true, // isAutomatable
                                                                   "Q",
                                                                     AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
                                                                        if (approximatelyEqual (value, 0.1f))
                                                                            return "0.1";
            
                                                                        if (approximatelyEqual (value, 10.f))
                                                                            return "10";

                                                                        return String(value,1);
                                                                   });
        
        normFreqParam = normFreq.get();
        normFreqParam->addListener (this);
        
        resParam = res.get();
        resParam->addListener (this);
        
        auto layout = createDefaultParameterLayout(false);
        layout.add (std::move (normFreq));
        layout.add (std::move (res));
        apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
    }
	
	~SEMFilter() override
    {
        normFreqParam->removeListener (this);
        resParam->removeListener (this);
    }
	
    void prepareToPlay (double Fs, int bufferSize) override
    {
        const ScopedLock sl (getCallbackLock());
        sampleRate = (float) Fs;
        normFreqSmooth.reset (sampleRate, 0.001);
        resSmooth.reset (sampleRate, 0.001);
        setRateAndBufferSizeDetails (Fs, bufferSize);
        updateCoefficients();
    }
    
    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("SEM Filter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "semFilter"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    
    void parameterValueChanged (int , float ) override
    {
        const ScopedLock sl (getCallbackLock());
        updateCoefficients();
    }
    
    void parameterGestureChanged (int, bool) override {}
    
    
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)  override  { process (buffer); }
    void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&) override  { process (buffer); }

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (approximatelyEqual (normFreqSmooth.getNextValue(),0.0f))
            setBypass (true);
        else
            setBypass (false);
            
        const bool isWholeProcBypassed = isBypassed()
                                      || buffer.hasBeenCleared()
                                      || numChannels <= 0
                                      || numSamples <= 0;

        const ScopedLock sl (getCallbackLock());

        if (!isWholeProcBypassed)
        {
            if (normFreqSmooth.getNextValue() < 0.f)
            {
                if (!prevBufferWasLPF)
                {
                    reset();
                    prevBufferWasLPF = true;
                }
                    
                SampleType out = 0;
                for (int c = 0 ; c < numChannels; ++c)
                {
                    for (int s = 0; s < numSamples; ++s)
                    {
                        out = processSampleLPF (buffer.getWritePointer (c)[s],c);
                        buffer.getWritePointer (c)[s] = out;
                    }
                }
            }
            else
            {
                SampleType out = 0;
                for (int c = 0 ; c < numChannels; ++c)
                {
                    for (int s = 0; s < numSamples; ++s)
                    {
                        out = processSampleHPF (buffer.getWritePointer (c)[s],c);
                        buffer.getWritePointer (c)[s] = out;
                    }
                }
            }
        }
    }
    
    template<typename SampleType>
    SampleType processSampleLPF(SampleType in, int channel)
	{
        
        // Filter Prep
        SampleType sigma = s1[channel]*beta1 + s2[channel]*beta2 + s3[channel]*beta3 + s4[channel]*beta4;
        
        SampleType u = in * (1.f+K);
        u = (u - sigma*(K)) * alpha0;
        u = std::tanh (1.2f*u);
        
        // LPF1
        SampleType x1 = alpha * (u - s1[channel]);
        SampleType y1 = s1[channel] + x1;
        s1[channel] = x1 + y1;
        
        SampleType x2 = alpha * (y1 - s2[channel]);
        SampleType y2 = s2[channel] + x2;
        s2[channel] = x2 + y2;
        
        SampleType x3 = alpha * (y2 - s3[channel]);
        SampleType y3 = s3[channel] + x3;
        s3[channel] = x3 + y3;
        
        SampleType x4 = alpha * (y3 - s4[channel]);
        SampleType y4 = s4[channel] + x4;
        s4[channel] = x4 + y4;
        
        return y4;
	}
    
    template<typename SampleType>
    SampleType processSampleHPF(SampleType x, int channel)
    {
        
        SampleType x1 = alpha * (x - s1[channel]);
        SampleType x2 = x1 + s1[channel];
        s1[channel] = x1 + x2;
        SampleType y1 = x - x2;
        
        SampleType u = alpha0 * (y1 + s3[channel]*beta3 + s2[channel]*beta2);
        SampleType output = u;
        SampleType y = K * u;
        u = std::tanh(1.5f*u);
        
        SampleType x3 = alpha * (y - s2[channel]);
        SampleType x4 = x3 + s2[channel];
        s2[channel] = x3 + x4;
        
        SampleType x5 = alpha * (y - x4 - s3[channel]);
        SampleType y3 = s3[channel] + x5;
        s3[channel] = y3 + x5;

        return output;
    }

    void setNormFreq(float newNormFreq){
        
        normFreqParam->setValueNotifyingHost (newNormFreq);
        updateCoefficients();
        
    }
    
    // Allowable range from 0.01f to ~10
    void setQValue(float q){
        resParam->setValueNotifyingHost (q);
        updateCoefficients();
    }
    
    
    
    void reset() override
    {
        s1[0] = 0; s1[1] = 0;
        s2[0] = 0; s2[1] = 0;
        s3[0] = 0; s3[1] = 0;
        s4[0] = 0; s4[1] = 0;
    }

private:
    
    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    NotifiableAudioParameterFloat* normFreqParam = nullptr;
    NotifiableAudioParameterFloat* resParam = nullptr;
    
    SmoothedValue <float, ValueSmoothingTypes::Linear> normFreqSmooth { 0.0f };
    SmoothedValue <float, ValueSmoothingTypes::Linear> resSmooth { 0.7071f };
    
    float sampleRate;
    float K; // transformed bandwidth "Q"
    float g;
    float G;
    float gamma;
    float alpha;
    float alpha0;
    float beta4;
    float beta3;
    float beta2;
    float beta1;
    float s1[2] = {0.0f};
    float s2[2] = {0.0f};
    float s3[2] = {0.0f};
    float s4[2] = {0.0f};
    
    bool prevBufferWasLPF = true;
    
    void updateCoefficients()
    {
        
        normFreqSmooth.setTargetValue (normFreqParam->get());
        resSmooth.setTargetValue (resParam->get());
        
        if (normFreqSmooth.getNextValue() <= 0.f) // LPF
        {
            if (!prevBufferWasLPF)
            {
                reset();
                prevBufferWasLPF = false;
            }
            
            K = resSmooth.getNextValue() /3.9f;
            float posFreq = normFreqSmooth.getNextValue() + 1.f;
            K = K + 0.3f*posFreq;  //scaling of K to keep resonance similar across spectrum
            
            float freqHz = 2.f * std::powf (10.f,3.f * posFreq + 1.f);
            float wd = 2.f * (float) M_PI * freqHz;
            float T = 1.f/sampleRate;
            float wa = (2.f/T) * std::tan(wd*T/2.f); // Warping for BLT
            g = wa * T/2.f;
            G = g/(1.f + g);
            gamma = G*G*G*G;
            alpha = G;
            alpha0 = 1.f / (1.f + K * gamma);
            beta1 = (G * G * G)/(1.f + g);
            beta2 = (G * G)/(1.f + g);
            beta3 = (G)/(1.f + g);
            beta4 = (1.f)/(1.f + g);
        }
        else // HPF
        {
            if (prevBufferWasLPF)
            {
                reset();
                prevBufferWasLPF = false;
            }
            
            // For HPF, K should have a range of 0.1 to 1 (max resonance)
            K = (((resSmooth.getNextValue()-0.1f) / 9.9f) * 0.9f) + 0.1f;
            
            float freqHz = 2.f * std::powf(10.f,3.f * normFreqSmooth.getNextValue() + 1.f);
            float wd = 2.f * (float) M_PI * freqHz;
            float T = 1.f/sampleRate;
            float wa = (2.f/T) * std::tan(wd*T/2.f); // Warping for BLT
            g = wa*T/2.f;
            G = g/(1.f + g);
            gamma = G*G*G*G;
            alpha = G;
            alpha0 = 1.f / (1.f - K * G + K*G*G);
            //beta1 = (G * G * G)/(1.f + g);
            beta2 = (1.f)/(1.f + g);
            beta3 = -G/(1.f + g);
            //beta4 = (1.f)/(1.f + g);
        }

        
    }
};
