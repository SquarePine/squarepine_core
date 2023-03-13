
// This model is based on the Virtual Analog analysis
// by Will Pirkle: https://github.com/ddiakopoulos/MoogLadders/blob/master/src/OberheimVariationModel.h

class SEMLowPassFilter
{
public:
    void prepareToPlay (double Fs, int)
    {
        sampleRate = (float) Fs;
        normFreqSmooth.reset (sampleRate, 0.0001f);
        resSmooth.reset (sampleRate, 0.0001f);
        updateCoefficients();
    }

    void setNormFreq (float newNormFreq)
    {
        if (targetFreq != newNormFreq)
        {
            targetFreq = newNormFreq;
            normFreqSmooth.setTargetValue (targetFreq);
            updateCoefficients();
        }
    }

    // Allowable range from 0.01f to ~10
    void setQValue (float q)
    {
        if (targetRes != q)
        {
            targetRes = q;
            resSmooth.setTargetValue (targetRes);
            updateCoefficients();
        }
    }

    float processSample (float in, int channel)
    {
        resSmooth.getNextValue();
        normFreqSmooth.getNextValue();
        // Filter Prep
        float sigma = s1[channel] * beta1 + s2[channel] * beta2 + s3[channel] * beta3 + s4[channel] * beta4;

        float u = in * (1.f + K);
        u = (u - sigma * (K)) * alpha0;
        u = std::tanh (1.2f * u);

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
private:
    SmoothedValue<float, ValueSmoothingTypes::Linear> normFreqSmooth { 1.0f };
    SmoothedValue<float, ValueSmoothingTypes::Linear> resSmooth { 0.7071f };

    float targetFreq = 0.0f;
    float targetRes = 0.1f;

    float sampleRate;
    float K;// transformed bandwidth "Q"
    float g;
    float G;
    float gamma;
    float alpha;
    float alpha0;
    float beta4;
    float beta3;
    float beta2;
    float beta1;
    float s1[2] = { 0.0f };
    float s2[2] = { 0.0f };
    float s3[2] = { 0.0f };
    float s4[2] = { 0.0f };

    void updateCoefficients()
    {
        K = resSmooth.getNextValue() / 3.9f;
        float normFreq = normFreqSmooth.getNextValue();
        K = K + 0.3f * normFreq;//scaling of K to keep resonance similar across spectrum

        float freqHz = 2.f * std::powf (10.f, 3.f * normFreq + 1.f);
        float wd = 2.f * (float) M_PI * freqHz;
        float T = 1.f / sampleRate;
        float wa = (2.f / T) * std::tan (wd * T / 2.f);// Warping for BLT
        g = wa * T / 2.f;
        G = g / (1.f + g);
        gamma = G * G * G * G;
        alpha = G;
        alpha0 = 1.f / (1.f + K * gamma);
        beta1 = (G * G * G) / (1.f + g);
        beta2 = (G * G) / (1.f + g);
        beta3 = (G) / (1.f + g);
        beta4 = (1.f) / (1.f + g);
    }
};

class SEMHighPassFilter
{
public:
    void prepareToPlay (double Fs, int)
    {
        sampleRate = (float) Fs;
        normFreqSmooth.reset (sampleRate, 0.0001);
        resSmooth.reset (sampleRate, 0.0001);
        updateCoefficients();
    }

    void setNormFreq (float newNormFreq)
    {
        if (targetFreq != newNormFreq)
        {
            targetFreq = newNormFreq;
            normFreqSmooth.setTargetValue (targetFreq);
            updateCoefficients();
        }
    }

    // Allowable range from 0.01f to ~10
    void setQValue (float q)
    {
        if (targetRes != q)
        {
            targetRes = q;
            resSmooth.setTargetValue (targetRes);
            updateCoefficients();
        }
    }

    float processSample (float x, int channel)
    {
        resSmooth.getNextValue();
        normFreqSmooth.getNextValue();

        float x1 = alpha * (x - s1[channel]);
        float x2 = x1 + s1[channel];
        s1[channel] = x1 + x2;
        float y1 = x - x2;

        float u = alpha0 * (y1 + s3[channel] * beta3 + s2[channel] * beta2);
        float output = u;
        float y = K * u;
        u = std::tanh (1.5f * u);

        float x3 = alpha * (y - s2[channel]);
        float x4 = x3 + s2[channel];
        s2[channel] = x3 + x4;

        float x5 = alpha * (y - x4 - s3[channel]);
        float y3 = s3[channel] + x5;
        s3[channel] = y3 + x5;

        return output;
    }
private:
    SmoothedValue<float, ValueSmoothingTypes::Linear> normFreqSmooth { 0.0f };
    SmoothedValue<float, ValueSmoothingTypes::Linear> resSmooth { 0.7071f };

    float targetFreq = 0.0f;
    float targetRes = 0.1f;

    float sampleRate;
    float K;// transformed bandwidth "Q"
    float g;
    float G;
    float gamma;
    float alpha;
    float alpha0;
    //float beta4;
    float beta3;
    float beta2;
    //float beta1;
    float s1[2] = { 0.0f };
    float s2[2] = { 0.0f };
    float s3[2] = { 0.0f };
    //float s4[2] = {0.0f};

    void updateCoefficients()
    {
        // For HPF, K should have a range of 0.1 to 1 (max resonance)
        K = (((resSmooth.getNextValue() - 0.1f) / 9.9f) * 0.9f) + 0.1f;

        float freqHz = 2.f * std::powf (10.f, 3.f * normFreqSmooth.getNextValue() + 1.f);
        float wd = 2.f * (float) M_PI * freqHz;
        float T = 1.f / sampleRate;
        float wa = (2.f / T) * std::tan (wd * T / 2.f);// Warping for BLT
        g = wa * T / 2.f;
        G = g / (1.f + g);
        gamma = G * G * G * G;
        alpha = G;
        alpha0 = 1.f / (1.f - K * G + K * G * G);
        //beta1 = (G * G * G)/(1.f + g);
        beta2 = (1.f) / (1.f + g);
        beta3 = -G / (1.f + g);
        //beta4 = (1.f)/(1.f + g);
    }
};

// Added this class temporarily for the HPF. Eventually we will want to use a better model of the SEM HPF
class DigitalFilter
{
public:
    enum FilterType
    {
        LPF,
        HPF,
        BPF1,
        BPF2,
        NOTCH,
        LSHELF,
        HSHELF,
        PEAK,
        APF
    };

    void setFilterType (FilterType filterTypeParam)
    {
        this->filterType = filterTypeParam;
    }
    
    void processBuffer (juce::AudioBuffer<float>& buffer, MidiBuffer&)
    {
        for (int c = 0 ; c < buffer.getNumChannels() ; ++c)
        {
            for (int n = 0 ; n < buffer.getNumSamples() ; ++n)
            {
                float x = buffer.getWritePointer (c)[n];
                float y = processSample (x,c);
                buffer.getWritePointer (c)[n] = y;
            }
        }
    }
    
    double processSample(double x, int channel)
    {
        performSmoothing();

        // Output, processed sample (Direct Form 1)
        double y = b0 * x + b1 * x1[channel] + b2 * x2[channel]
                   + (-a1) * y1[channel] + (-a2) * y2[channel];

        x2[channel] = x1[channel];// store delay samples for next process step
        x1[channel] = x;
        y2[channel] = y1[channel];
        y1[channel] = y;

        return y;
    }

    void setFs (double newFs)
    {
        Fs = newFs;
        updateCoefficients();// Need to update if Fs changes
    }

    void setFreq (double newFreq)
    {
        freqTarget = jlimit (20.0, 20000.0, newFreq);
        updateCoefficients();
    }

    void setQ (double newQ)
    {
        qTarget = jlimit (0.1, 10.0, newQ);
        updateCoefficients();
    }

    void setAmpdB (double newAmpdB)
    {
        ampdB = newAmpdB;

        updateCoefficients();
    }
private:
    FilterType filterType = LPF;

    double Fs = 48000.0;// Sampling Rate

    // Variables for User to Modify Filter
    double freqTarget = 20.0;// frequency in Hz
    double freqSmooth = 20.0;
    double qTarget = 0.707;// Q => [0.1 - 10]
    double qSmooth = 0.707;
    double ampdB = 0.0;// Amplitude on dB scale

    // Variables for Biquad Implementation
    // 2 channels - L,R : Up to 2nd Order
    double x1[2] = { 0.0 };// 1 sample of delay feedforward
    double x2[2] = { 0.0 };// 2 samples of delay feedforward
    double y1[2] = { 0.0 };// 1 sample of delay feedback
    double y2[2] = { 0.0 };// 2 samples of delay feedback

    // Filter coefficients
    double b0 = 1.0;// initialized to pass signal
    double b1 = 0.0;// without filtering
    double b2 = 0.0;
    double a0 = 1.0;
    double a1 = 0.0;
    double a2 = 0.0;

    void performSmoothing()
    {
        float alpha = 0.9999f;
        freqSmooth = alpha * freqSmooth + (1.f - alpha) * freqTarget;
        qSmooth = alpha * qSmooth + (1.f - alpha) * qTarget;
    }
    void updateCoefficients()
    {
        double A = std::pow (10.0, ampdB / 40.0);// Linear amplitude

        // Normalize frequency
        double w0 = (2.0 * M_PI) * freqSmooth / Fs;

        // Bandwidth/slope/resonance parameter
        double alpha = std::sin (w0) / (2.0 * qSmooth);

        double cw0 = std::cos (w0);
        switch (filterType)
        {
            case LPF:
            {
                a0 = 1.0 + alpha;
                double B0 = (1.0 - cw0) / 2.0;
                b0 = B0 / a0;
                double B1 = 1.0 - cw0;
                b1 = B1 / a0;
                double B2 = (1.0 - cw0) / 2.0;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;
                break;
            }
            case HPF:
            {
                a0 = 1.0 + alpha;
                double B0 = (1.0 + cw0) / 2.0;
                b0 = B0 / a0;
                double B1 = -(1.0 + cw0);
                b1 = B1 / a0;
                double B2 = (1.0 + cw0) / 2.0;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;
                break;
            }
            case BPF1:
            {
                double sw0 = std::sin (w0);
                a0 = 1.0 + alpha;
                double B0 = sw0 / 2.0;
                b0 = B0 / a0;
                double B1 = 0.0;
                b1 = B1 / a0;
                double B2 = -sw0 / 2.0;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;
                break;
            }
            case BPF2:
            {
                a0 = 1.0 + alpha;
                double B0 = alpha;
                b0 = B0 / a0;
                double B1 = 0.0;
                b1 = B1 / a0;
                double B2 = -alpha;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;

                break;
            }
            case NOTCH:
            {
                a0 = 1.0 + alpha;
                double B0 = 1.0;
                b0 = B0 / a0;
                double B1 = -2.0 * cw0;
                b1 = B1 / a0;
                double B2 = 1.0;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;
                break;
            }
            case LSHELF:
            {
                double sqA = std::sqrt (A);
                a0 = (A + 1.0) + (A - 1.0) * cw0 + 2.0 * sqA * alpha;
                double B0 = A * ((A + 1.0) - (A - 1.0) * cw0 + 2.0 * sqA * alpha);
                b0 = B0 / a0;
                double B1 = 2.0 * A * ((A - 1.0) - (A + 1.0) * cw0);
                b1 = B1 / a0;
                double B2 = A * ((A + 1.0) - (A - 1.0) * cw0 - 2.0 * sqA * alpha);
                b2 = B2 / a0;
                double A1 = -2.0 * ((A - 1.0) + (A + 1.0) * cw0);
                a1 = A1 / a0;
                double A2 = (A + 1.0) + (A - 1.0) * cw0 - 2.0 * sqA * alpha;
                a2 = A2 / a0;

                break;
            }

            case HSHELF:
            {
                double sqA = std::sqrt (A);
                a0 = (A + 1.0) - (A - 1.0) * cw0 + 2.0 * sqA * alpha;
                double B0 = A * ((A + 1.0) + (A - 1.0) * cw0 + 2.0 * sqA * alpha);
                b0 = B0 / a0;
                double B1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cw0);
                b1 = B1 / a0;
                double B2 = A * ((A + 1.0) + (A - 1.0) * cw0 - 2.0 * sqA * alpha);
                b2 = B2 / a0;
                double A1 = 2.0 * ((A - 1.0) - (A + 1.0) * cw0);
                a1 = A1 / a0;
                double A2 = (A + 1.0) - (A - 1.0) * cw0 - 2.0 * sqA * alpha;
                a2 = A2 / a0;

                break;
            }

            case PEAK:

            {
                a0 = 1.0 + alpha / A;
                double B0 = 1.0 + alpha * A;
                b0 = B0 / a0;
                double B1 = -2.0 * cw0;
                b1 = B1 / a0;
                double B2 = 1.0 - alpha * A;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha / A;
                a2 = A2 / a0;

                break;
            }

            case APF:
            {
                a0 = 1.0 + alpha;
                double B0 = 1.0 - alpha;
                b0 = B0 / a0;
                double B1 = -2.0 * cw0;
                b1 = B1 / a0;
                double B2 = 1.0 + alpha;
                b2 = B2 / a0;
                double A1 = -2.0 * cw0;
                a1 = A1 / a0;
                double A2 = 1.0 - alpha;
                a2 = A2 / a0;

                break;
            }
        }
    }
};

// The SEMFilter in the DJDAW is a combination of both a LPF and HPF
// By changing the value of the cut-off parameter to be above 0 (halfway),
// the filter becomes a HPF. If the value is below 0, it is a LPF
class SEMFilter final : public InternalProcessor,
                        public AudioProcessorParameter::Listener
{
public:
    SEMFilter (int idNum=1)
        : idNumber (idNum)
    {
        reset();

        NormalisableRange<float> freqRange = { -1.f, 1.f };
        auto normFreq = std::make_unique<NotifiableAudioParameterFloat> ("freqSEM", "Frequency", freqRange, 0.0f,
                                                                         true,// isAutomatable
                                                                         "Cut-off",
                                                                         AudioProcessorParameter::genericParameter,
                                                                         [] (float value, int) -> String
                                                                         {
                                                                             if (approximatelyEqual (value, 0.0f))
                                                                                 return "BYP";

                                                                             if (value < 0.0f)
                                                                             {
                                                                                 float posFreq = value + 1.f;
                                                                                 float freqHz = 2.f * std::powf (10.f, 3.f * posFreq + 1.f);
                                                                                 return String (freqHz, 0);
                                                                             }
                                                                             else
                                                                             {
                                                                                 float freqHz = 2.f * std::powf (10.f, 3.f * value + 1.f);
                                                                                 return String (freqHz, 0);
                                                                             }
                                                                         });

        NormalisableRange<float> qRange = { 0.1f, 10.f };
        auto res = std::make_unique<NotifiableAudioParameterFloat> ("resSEM", "resonance", qRange, 0.7071f,
                                                                    true,// isAutomatable
                                                                    "Q",
                                                                    AudioProcessorParameter::genericParameter,
                                                                    [] (float value, int) -> String
                                                                    {
                                                                        if (approximatelyEqual (value, 0.1f))
                                                                            return "0.1";

                                                                        if (approximatelyEqual (value, 10.f))
                                                                            return "10";

                                                                        return String (value, 1);
                                                                    });

        setPrimaryParameter(normFreqParam);
        normFreqParam = normFreq.get();
        normFreqParam->addListener (this);

        resParam = res.get();
        resParam->addListener (this);

        auto layout = createDefaultParameterLayout (false);
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
        lpf.prepareToPlay (Fs, bufferSize);
        hpf.prepareToPlay (Fs, bufferSize);
        mixLPF.reset (Fs, 0.001f);
        mixHPF.reset (Fs, 0.001f);
        setRateAndBufferSizeDetails (Fs, bufferSize);

        hpf2.setFs (Fs);
        hpf2.setFreq (20.f);
        hpf2.setQ (std::sqrt (2.f));
        hpf2.setFilterType (DigitalFilter::FilterType::HPF);
    }

    //==============================================================================
    /** @internal */
    const String getName() const override { return TRANS ("SEM Filter"); }
    /** @internal */
    Identifier getIdentifier() const override { return "SEM Filter" + String (idNumber); }
    /** @internal */
    bool supportsDoublePrecisionProcessing() const override { return false; }

    void parameterValueChanged (int paramNum, float value) override
    {
        const ScopedLock sl (getCallbackLock());
        if (paramNum == 1)
        {// Frequency change
            lpf.setNormFreq (jmin (1.f, value + 1.f));
            hpf.setNormFreq (jmax (0.0001f, value));

            float freqHz = 2.f * std::powf (10.f, 3.f * jmax (0.0001f, value) + 1.f);
            hpf2.setFreq (freqHz);

            if (value < 0.f)
            {
                mixLPF.setTargetValue (1.f);
                mixHPF.setTargetValue (0.f);
            }
            else if (value > 0.f)
            {
                mixLPF.setTargetValue (0.f);
                mixHPF.setTargetValue (1.f);
            }
            else
            {
                mixLPF.setTargetValue (0.f);
                mixHPF.setTargetValue (0.f);
            }
        }
        else
        {// Resonance change
            lpf.setQValue (value);
            hpf.setQValue (value);

            hpf2.setQ (value);
        }
    }

    void parameterGestureChanged (int, bool) override {}

    void processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer&) override { process (buffer); }
    //void processBlock (juce::AudioBuffer<double>& buffer, MidiBuffer&) override  { process (buffer); }

    void process (juce::AudioBuffer<float>& buffer)
    {
        if(isBypassed())return;
        const auto numChannels = buffer.getNumChannels();
        const auto numSamples = buffer.getNumSamples();

        const ScopedLock sl (getCallbackLock());

        float x, y, mix;
        for (int c = 0; c < numChannels; ++c)
        {
            for (int s = 0; s < numSamples; ++s)
            {
                mix = mixLPF.getNextValue();
                x = buffer.getWritePointer (c)[s];
                y = (1.f - mix) * x + mix * lpf.processSample (x, c);

                mix = mixHPF.getNextValue();
                y = (1.f - mix) * y + mix * (float) hpf2.processSample (y, c);
                buffer.getWritePointer (c)[s] = y;
            }
        }
    }

    void setNormFreq (float newNormFreq)
    {
        normFreqParam->setValueNotifyingHost (newNormFreq);
        lpf.setNormFreq (newNormFreq);
        hpf.setNormFreq (newNormFreq);

        if (newNormFreq < 0.f)
        {
            mixLPF.setTargetValue (1.f);
            mixHPF.setTargetValue (0.f);
        }
        else if (newNormFreq > 0.f)
        {
            mixLPF.setTargetValue (0.f);
            mixHPF.setTargetValue (1.f);
        }
        else
        {
            mixLPF.setTargetValue (0.f);
            mixHPF.setTargetValue (0.f);
        }
    }

    // Allowable range from 0.01f to ~10
    void setQValue (float q)
    {
        resParam->setValueNotifyingHost (q);
        lpf.setQValue (q);
        hpf.setQValue (q);
    }
    void setID (int idNum)
    {
        idNumber = idNum;
    }
private:
    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    NotifiableAudioParameterFloat* normFreqParam = nullptr;
    NotifiableAudioParameterFloat* resParam = nullptr;

    SmoothedValue<float, ValueSmoothingTypes::Linear> mixLPF { 0.0f };
    SmoothedValue<float, ValueSmoothingTypes::Linear> mixHPF { 0.0f };

    int idNumber = 1;

    SEMLowPassFilter lpf;
    SEMHighPassFilter hpf;
    DigitalFilter hpf2;// Use for now. Swap out later for a better model of  the SEMHighPassFilter
};
