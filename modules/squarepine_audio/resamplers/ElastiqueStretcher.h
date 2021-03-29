#if SQUAREPINE_USE_ELASTIQUE

}

class CElastiqueProV3If;

namespace sp
{

class ElastiqueStretcher : public Stretcher
{
public:
    ElastiqueStretcher (double sampleRate, int outputLength,
                        double stretch = 1.0, double pitch = 1.0,
                        bool exactStretch = true) noexcept;

    ~ElastiqueStretcher();

    int getInputLength() const override;
    int getInputLength (int newOutputLength) override;
    int getMaxInputLength() const override;
    void reset (double sampleRate, int outBufferSize) override;
    void process (AudioBuffer<float>& buffer) override;
    int getRemainingSamples (AudioBuffer<float>& buffer) override;

private:
    CElastiqueProV3If* elastiqueInstance = nullptr;
    AudioBuffer<float> internalBuffer;

    void update (bool exactStretch) override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ElastiqueStretcher)
};

#endif //SQUAREPINE_USE_ELASTIQUE
