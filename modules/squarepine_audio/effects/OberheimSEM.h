
// This model is based on the Virtual Analog analysis
// by Will Pirkle: https://github.com/ddiakopoulos/MoogLadders/blob/master/src/OberheimVariationModel.h


class OberheimSEM final : public InternalProcessor,
                           public AudioProcessorParameter::Listener
{
public:
	
	OberheimSEM()
    {
        reset();
        
        NormalisableRange<float> freqRange = { -1.f, 1.f};
        auto normFreq = std::make_unique<NotifiableAudioParameterFloat> ("freq","Frequency",
                                                                   freqRange,
                                                                   0.0f,
                                                                   true, // isAutomatable
                                                                   "Cut-off",
                                                                     AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
                                                                        if (approximatelyEqual (value,0.0f))
                                                                            return "BYPASS";
                                                                            
                                                                        if (value < 0.0f)
                                                                        {
                                                                            float posValue = value + 1.0f;
                                                                            float freqHz = 2.f * std::powf(10.f,3.f*posValue + 1.f);
                                                                            return String(freqHz,0);
                                                                        }
                                                                        if (value > 0.0f)
                                                                        {
                                                                            float freqHz = 2.f * std::powf(10.f,3.f*value + 1.f);
                                                                            return String(freqHz,0);
                                                                        }
                                                                   });
        
        NormalisableRange<float> qRange = { 0.1f, 20.f};
        auto res = std::make_unique<NotifiableAudioParameterFloat> ("res","resonance",
                                                                   qRange,
                                                                   0.7071f,
                                                                   true, // isAutomatable
                                                                   "Q",
                                                                     AudioProcessorParameter::genericParameter,
                                                                   [] (float value, int) -> String
                                                                   {
                                                                        if (approximatelyEqual (value, 0.1f))
                                                                            return "0.1";
            
                                                                        if (approximatelyEqual (value, 20.f))
                                                                            return "20";

                                                                        return String(value,1);
                                                                   });
        
        normFreqParam = normFreq.get();
        normFreqParam->addListener (this);
        
        resParam = res.get();
        resParam->addListener (this);
        
        auto layout = createDefaultParameterLayout();
        layout.add (std::move (normFreq));
        layout.add (std::move (res));
        apvts.reset (new AudioProcessorValueTreeState (*this, nullptr, "parameters", std::move (layout)));
    }
	
	~OberheimSEM() override
    {
        normFreqParam->removeListener (this);
        resParam->removeListener (this);
    }
	
    void prepareToPlay (double Fs, int bufferSize) override
    {
        setRateAndBufferSizeDetails (Fs, bufferSize);
        sampleRate = (float) Fs;
        updateCoefficients();
    }
    
    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("Oberheim Filter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "oberheim"; }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return true; }
    
    void parameterValueChanged (int, float) override
    {
        // smoothing?
        
    }
    
    void parameterGestureChanged (int, bool) override {}
    
    
    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&)  override  { process (buffer); }
    void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&) override  { process (buffer); }

    template<typename SampleType>
    void process (juce::AudioBuffer<SampleType>& buffer)
    {
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        if (approximatelyEqual (normFreqParam->get(),0.f))
            setBypass(true);
            
        const bool isWholeProcBypassed = isBypassed()
                                      || buffer.hasBeenCleared()
                                      || numChannels <= 0
                                      || numSamples <= 0;

        const ScopedLock sl (getCallbackLock());

        if (!isWholeProcBypassed)
        {
            if (normFreqParam->get() < 0.0)
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
                        out = processSampleLPF(buffer.getWritePointer(c)[s],c);
                        buffer.getWritePointer(c)[s] = out;
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
                        out = processSampleHPF(buffer.getWritePointer(c)[s],c);
                        buffer.getWritePointer(c)[s] = out;
                    }
                }
            }
        }
    }
    
	float processSampleLPF(float in, int channel)
	{
        
        // Filter Prep
        float sigma = s1[channel]*beta1 + s2[channel]*beta2 + s3[channel]*beta3 + s4[channel]*beta4;
        
        float u = in * (1.f+K);
        u = (u - sigma*(K)) * alpha0;
        u = std::tanh (1.2f*u);
        
        // LPF1
        float x1 = alpha * (u - s1[channel]);
        float y1 = s1[channel] + x1;
        s1[channel] = x1 + y1;
        
        float x2 = alpha * (y1 - s2[channel]);
        float y2 = s2[channel] + x2;
        s2[channel] = x2 + y2;
        
        float x3 = alpha * (y2 - s3[channel]);
        float y3 = s3[channel] + x3;
        s3[channel] = x3 + y3;
        
        float x4 = alpha * (y3 - s4[channel]);
        float y4 = s4[channel] + x4;
        s4[channel] = x4 + y4;
        
        return y4;
	}
    
    float processSampleHPF(float x, int channel)
    {
        
        float x1 = alpha * (x - s1[channel]);
        float x2 = x1 + s1[channel];
        s1[channel] = x1 + x2;
        float y1 = x - x2;
        
        float u = alpha0 * (y1 + s3[channel]*beta3 + s2[channel]*beta2);
        float output = u;
        float y = K * u;
        u = std::tanh(1.5f*u);
        
        float x3 = alpha * (y - s2[channel]);
        float x4 = x3 + s2[channel];
        s2[channel] = x3 + x4;
        
        float x5 = alpha * (y - x4 - s3[channel]);
        float y3 = s3[channel] + x5;
        s3[channel] = y3 + x5;

        return output;
    }

    void setNormFreq(float newNormFreq){
        
        if (normFreqParam->get() != newNormFreq)
        {
            normFreqParam->setValueNotifyingHost (newNormFreq);
            updateCoefficients();
        }
        
    }
    
    // Allowable range from 0.01f to ~20
    void setQValue(float q){
        if (resParam->get() != q)
        {
            resParam->setValueNotifyingHost (q);
            if (normFreqParam->get() < 0.0)
            {
                K = q/3.9f;
                float posFreq = normFreqParam->get() + 1.f;
                //float freqHz = normFreq->get()
                //float freq0to1 = log10(freqHz/8.0f)/3.439332693830263f;
                K = K + 0.3f*posFreq;  //scaling of K to keep resonance similar across spectrum
            }
            else
            {
                // For HPF, K should have a range of 0.1 to 1.9 (max resonance)
                K = (((resParam->get()-0.1f) / 19.9f) * 1.8f) + 0.1f;
            }
            updateCoefficients();
        }
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
        if (normFreqParam->get() >= 0.0) // LPF
        {
            if (!prevBufferWasLPF)
            {
                reset();
                prevBufferWasLPF = false;
            }
            
            float posFreq = normFreqParam->get() + 1.f;
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
            
            float freqHz = 2.0 * std::powf(10.f,3.f * normFreqParam->get() + 1.f);
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
