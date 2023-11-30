/** */
template<typename FloatType>
class EnvelopeFollower final
{
public:
    /** */
    EnvelopeFollower (FloatType rate = FloatType (48000)) noexcept
    {
        reset (rate);
    }

    /** */
    void setAttackMs (FloatType ms) noexcept
    {
        attack.set (ms, sampleRate);
    }

    /** */
    void setReleaseMs (FloatType ms) noexcept
    {
        release.set (ms, sampleRate);
    }

    /** */
    void reset (FloatType rate, FloatType initialEnvelope = 0) noexcept
    {
        jassert (rate > 0);
        sampleRate = rate;
        envelope = initialEnvelope;
        setAttackMs (attack.timeMs);
        setReleaseMs (release.timeMs);
    }

    /** */
    [[nodiscard]] FloatType processDecibels (FloatType sample) noexcept
    {
        EnvelopeComponent& ec = sample > envelope ? attack : release;
        envelope = ec.process (envelope, sample);
        return envelope;
    }

    /** */
    [[nodiscard]] FloatType process (FloatType sample) noexcept
    {
        sample = std::abs (sample);
        return processDecibels (sample);
    }

private:
    /** */
    struct EnvelopeComponent final
    {
        /** */
        void set (FloatType newTimeMs, FloatType sampleRate_) noexcept
        {
		    timeMs = std::max (newTimeMs, (FloatType) 1e-15);
            coefficient = std::exp (std::log ((FloatType) 0.01) / (timeMs * sampleRate_ * (FloatType) 0.001));
        }

        /** */
        [[nodiscard]] FloatType process (FloatType sample, FloatType envelope_) const noexcept
        {
            return coefficient * (envelope_ - sample) + sample;
        }

        FloatType timeMs = (FloatType) 1,
                  coefficient = (FloatType) 1;
    };

    FloatType sampleRate = (FloatType) 48000,
              envelope = (FloatType) 0;
    EnvelopeComponent attack, release;

    JUCE_DECLARE_NON_COPYABLE (EnvelopeFollower)
};
