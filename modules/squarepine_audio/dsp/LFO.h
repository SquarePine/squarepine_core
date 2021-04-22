/** */
class LFO
{
public:
    LFO();
    virtual ~LFO();

    //==============================================================================
    class Operation
    {
    public:
        Operation() noexcept;
        virtual ~Operation() noexcept;

        virtual void perform (double& sample, double value) = 0;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Operation)
    };

    #define CREATE_OPERATION(name) \
        class name : public Operation \
        { \
        public: \
            name() noexcept; \
            void perform (double& sample, double value) override; \
        \
        private: \
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (name) \
        }; \

    CREATE_OPERATION (AddOperation)
    CREATE_OPERATION (SubtractOperation)
    CREATE_OPERATION (MultiplyOperation)
    CREATE_OPERATION (DivideOperation)
    CREATE_OPERATION (EqualsOperation)

    #undef CREATE_OPERATION

    //==============================================================================
    void setOperation (Operation* newOperation);

    //==============================================================================
    class Configuration
    {
    public:
        Configuration() noexcept;
        Configuration (double sampleRate, double frequency, double phase) noexcept;
        Configuration (const Configuration& other) noexcept;
        Configuration& operator= (const Configuration& other) noexcept;

        //==============================================================================
        void prepare (double sampleRate, double frequency);

        //==============================================================================
        double getPhasePerSample() const noexcept { return MathConstants<double>::twoPi / (sampleRate / frequency); }

        //==============================================================================
        double sampleRate = 44100.0,
               frequency = 440.0,
               currentPhase = 0.0,
               amplitude = 0.0;

    private:
        //==============================================================================
        JUCE_LEAK_DETECTOR (Configuration)
    };

    //==============================================================================
    /** @returns Last phase, to keep track of where the wave is */
    double process (juce::AudioBuffer<float>& buffer, const Configuration& configuration);

protected:
    //==============================================================================
    virtual double calculateNextPhase (double currentPhase) const = 0;

private:
    //==============================================================================
    std::unique_ptr<Operation> operation;

    //==============================================================================
    void processInternal (float& sample, float phase);
    void processInternal (double& sample, double phase);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LFO)
};

//==============================================================================
/** */
class SineLFO final: public LFO
{
public:
    SineLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SineLFO)
};

//==============================================================================
/** */
class CosineLFO final : public LFO
{
public:
    CosineLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CosineLFO)
};

//==============================================================================
/** */
class TangentLFO final : public LFO
{
public:
    TangentLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TangentLFO)
};

//==============================================================================
/** */
class TriangleLFO : public LFO
{
public:
    TriangleLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriangleLFO)
};

//==============================================================================
/** */
class SawLFO : public LFO
{
public:
    SawLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SawLFO)
};

//==============================================================================
/** */
class RampLFO : public LFO
{
public:
    RampLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RampLFO)
};

//==============================================================================
/** */
class SquareLFO : public LFO
{
public:
    SquareLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquareLFO)
};

//==============================================================================
/** */
class WhiteNoiseLFO : public LFO
{
public:
    WhiteNoiseLFO();

protected:
    //==============================================================================
    /** @internal */
    double calculateNextPhase (double currentPhase) const override;

private:
    //==============================================================================
    const Random random;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WhiteNoiseLFO)
};
