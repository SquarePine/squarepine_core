class Stretcher
{
public:
    virtual ~Stretcher() noexcept;

    //==============================================================================
    /** @returns the required input vector length for the current output length */
    virtual int getInputLength() const = 0;
    virtual int getInputLength (int newOutputLength) = 0;
    virtual int getMaxInputLength() const = 0;

    /** Reset algorithm state */
    virtual void reset (double sampleRate, int outBufferSize) = 0;

    /** Process a block of audio.
     
        The buffer should be allocated by the caller. Only the first getInputLength()
        will be read, and the buffer should be at least outputLength samples long.
    */
    virtual void process (juce::AudioBuffer<float>& buffer) = 0;

    /** Gets samples remaining in the stretcher engine */
    virtual int getRemainingSamples (juce::AudioBuffer<float>& buffer) = 0;

    //==============================================================================
    /** Set pitch and stretch simultaneously.

        @param stretch      Set the pitch shift factor, Range: 0.1 to 10.0
        @param pitch        Set the time stretch factor. Range: 0.1 to 10.0
        @param exactStretch If true, will set exact stretch and quantise pitch and vice-versa.
    */
    void setStretchAndPitch (double stretch, double pitch, bool exactStretch);

    /**  Set exact stretch */
    void setStretch (double);
    double getStretch() const noexcept { return stretchFactor; }

    /** Set exact pitch */
    void setPitch (double);
    double getPitch() const noexcept { return pitchFactor; }

    double getSampleRate() const noexcept { return sampleRate; }
    int getOutputLength() const noexcept { return outputLength; }

protected:
    //==============================================================================
    Stretcher (double sampleRate_, int outputLength,
               double stretch = 1.0, double pitch = 1.0) noexcept;

    //==============================================================================
    /** Gets called when stretch or pitch factor has changed.

        Some stretching algorithms may quantize pitch or strech factors,
        even when only one of them is changed. Use this method to keep
        the interfaces's stretch and pitch variables consistent with
        the algorithm state.

        If "exactStretch" is true, "strechFactor" should be set exact and
        "pitchFactor" quantised, if false, the opposite.
    */
    virtual void update (bool exactStretch) = 0;

    //==============================================================================
    double sampleRate = 44100.0;
    int outputLength = 0;
    double stretchFactor = 1.0; //< Linear time stretching scaling factor: 0.1 to 10.0.
    double pitchFactor = 1.0;   //< Linear pitch shifting scaling factor: 0.1 to 10.0.

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Stretcher)
};
