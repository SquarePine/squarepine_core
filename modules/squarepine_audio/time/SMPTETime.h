/** Object denoting time in SMPTE format */
class SMPTETime final : public TimeFormat
{
public:
    /** */
    SMPTETime (int hours = 0, int minutes = 0, int seconds = 0,
               int frames = 0, double frameRate = 60.0) noexcept;
    /** */
    SMPTETime (const SMPTETime& smpteTime) noexcept;
    /** */
    ~SMPTETime() noexcept override;

    //==============================================================================
    /** */
    static SMPTETime fromSeconds (double timeSeconds, double frameRate = 60.0) noexcept;

    //==============================================================================
    /** */
    void setFrameRate (MidiMessage::SmpteTimecodeType rate) noexcept;
    /** */
    void setFrameRate (AudioPlayHead::FrameRateType rate) noexcept;

    //==============================================================================
    /** */
    double toSeconds() const override;

    /** @returns a String that will appear in the format "hh:mm:ss:ff" or "hh:mm:ss.ff" */
    String toString (bool showFrames) const;

    //==============================================================================
    /** */
    static double toDouble (MidiMessage::SmpteTimecodeType rate) noexcept;
    /** */
    static double toDouble (AudioPlayHead::FrameRateType rate) noexcept;

    //==============================================================================
    /** */
    SMPTETime& operator= (const SMPTETime& other) noexcept;
    /** */
    bool operator== (const SMPTETime& other) const noexcept;
    /** */
    bool operator!= (const SMPTETime& other) const noexcept;

    //==============================================================================
    int hours, minutes, seconds, frames;
    double frameRate;

private:
    //==============================================================================
    void moveIntoRange();

    /** @returns a String that will appear in the format "hh:mm:ss" or "hh:mm:ss" */
    String toString() const override;

    //==============================================================================
    JUCE_LEAK_DETECTOR (SMPTETime)
};
