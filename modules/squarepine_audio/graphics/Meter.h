/** */
struct DecibelHelpers
{
    enum
    {
        // The maximum gain value of the slider in decibels.
        maxSliderLevelDb = 12,

        // The minimum decibel level of the slider.
        minSliderLevelDb = -100,

        // The magnitude of spatial compression of lower decibel values versus higher values.
        // Ranges from 0 (linear) to infinity (hard knee).
        curveTensionDb = 6
    };

    /** @returns the point at which the volume slider should snap.

        @param snapDb The value in decibels which this slider should snap to
    */
    static double calculateSnapPoint (double snapDb) noexcept;

    /** @returns the proportion of the meter length for the gain value.

        @param gain            The absolute volume level.
        @param minimumDecibels The minimum level of the meter in decibels.
        @param maximumDecibels The maximum level of the meter in decibels.
    */
    static double gainToMeterProportion (double gain,
                                         int minimumDecibels = minSliderLevelDb,
                                         int maximumDecibels = maxSliderLevelDb) noexcept;

    /** @returns the gain value for the porportion of the meter length.

        @param meterProportion The proportion of the meter length.
        @param minimumDecibels The minimum level of the meter in decibels.
        @param maximumDecibels The maximum level of the meter in decibels.
    */
    static double meterProportionToGain (double meterProportion,
                                         int minimumDecibels = minSliderLevelDb,
                                         int maximumDecibels = maxSliderLevelDb) noexcept;

    /** @returns the proportion of the meter length for the decibel value.
* 
        @param meterProportion The proportion of the meter length.
        @param minimumDecibels The minimum level of the meter in decibels.
        @param maximumDecibels The maximum level of the meter in decibels.
    */
    static double meterProportionToDecibels (double meterProportion,
                                             int minimumDecibels = minSliderLevelDb,
                                             int maximumDecibels = maxSliderLevelDb) noexcept;

    /** @returns the proportion of the meter height for the decibel value.
* 
        @param decibels        The volume value in decibels.
        @param minimumDecibels The minimum level of the meter in decibels
        @param maximumDecibels The maximum level of the meter in decibels
    */
    static double decibelsToMeterProportion (double decibels,
                                             int minimumDecibels = minSliderLevelDb,
                                             int maximumDecibels = maxSliderLevelDb) noexcept;

    /** Maps values on [0,1] onto a tension curve of the same interval.

        Used to spatially differentiate higher decibel values.

        The curve compresses lower values and expands higher values:
        1 |     /'
          |    / `
          |   / ,
          |  / .
          | /,
          |/`_____
        0         1
        
        @param value: the value to transform.
    */
    static double linearToCurved (double value) noexcept;

    /** @returns the inverse of linearToCurved().

        This curve expandes lower values and compresses higher values:
        1 |    ,/
          |  ' /
          | ` /
          |. /
          |,/
          |/_____
        0        1
    
        @param curvedValue: the value to transform.
    */
    static double curvedToLinear (double curvedValue) noexcept;

private:
    SQUAREPINE_DECLARE_TOOL_CLASS (DecibelHelpers)
};

//==============================================================================
/** */
class Meter final : public Component
{
public:
    //==============================================================================
    /** */
    class Model
    {
    public:
        /** */
        virtual ~Model() noexcept = default;

        /** @returns the expiration time of the maximum meter level, after which it decays. */
        virtual int64 getExpiryTimeMs() const noexcept { return 3000; }

        /** @returns */
        virtual bool needsMaxLevel() const noexcept { return false; }

        /** @returns */
        virtual bool isHorizontal() const noexcept { return false; }

        /** @returns */
        virtual Array<float> getChannelLevels() const = 0;

        /** */
        struct ColourPosition final
        {
            ColourPosition() noexcept = default;

            ColourPosition (Colour c, double d) noexcept :
                colour (c),
                decibels (d)
            {
            }

            Colour colour;
            double decibels = 0.0;
        };

        /** @returns */
        virtual std::array<ColourPosition, 3> getColourPositions() const
        {
            return
            {
                ColourPosition (Colours::red, 0.0),
                ColourPosition (Colours::yellow, -9.0),
                ColourPosition (Colours::green, -18.0)
            };
        }
    };

    //==============================================================================
    /** */
    Meter (Model* model_ = nullptr);

    //==============================================================================
    /** */
    void setModel (Model*);

    /** */
    Model* getModel() const noexcept { return model; }

    //==============================================================================
    /** @returns true if the levels have changed. */
    bool refresh();

    //==============================================================================
    /** */
    struct ChannelContext
    {
        ChannelContext() = default;
        ChannelContext (const ChannelContext&) = default;
        ChannelContext (ChannelContext&&) = default;
        ~ChannelContext() = default;
        ChannelContext& operator= (const ChannelContext&) = default;
        ChannelContext& operator= (ChannelContext&&) = default;

        void setLevel (float newLevel) { set (level, newLevel); }
        float getLevel() const noexcept { return level; }

        void setLastLevel (float newLastLevel) { set (lastLevel, newLastLevel); }
        float getLastLevel() const noexcept { return lastLevel; }

        void setMaxLevel (float newMaxLevel) { set (maxLevel, newMaxLevel); }
        float getMaxLevel() const noexcept { return maxLevel; }

        void setLastMaxLevel (float newLastMaxLevel) { set (lastMaxLevel, newLastMaxLevel); }
        float getLastMaxLevel() const noexcept { return lastMaxLevel; }

        void setLastMaxAudioLevelTime (int64 t) { timeOfMaximumMs = t; }
        int64 getLastMaxAudioLevelTime() const noexcept { return timeOfMaximumMs; }

        void setMeterArea (const Rectangle<int>& area) { meterArea = area; }
        const Rectangle<int>& getMeterArea() const noexcept { return meterArea; }

    private:
        float level = 0.0f;         // The last measured audio absolute volume level.
        float lastLevel = 0.0f;     // The volume level of the last update, used to check if levels have changed for repainting.
        float maxLevel = 0.0f;      // The maximum audio levels of the trailing 3 seconds.
        float lastMaxLevel = 0.0f;  // The max volume level of the last update.
        int64 timeOfMaximumMs = 0;  // The time of the last maximum audio level.
        Rectangle<int> meterArea;   // The left/right drawable regions for the meter.

        void set (float& value, float newValue)
        {
            dsp::util::snapToZero (newValue);
            value = newValue;
        }
    };

    /** */
    const ChannelContext& getChannel (int channel) const noexcept { return channels.getReference (channel); }

    /** */
    float getChannelLevel (int channel) const noexcept { return getChannel (channel).getLevel(); }

    //==============================================================================
    /** */
    enum class ClippingLevel
    {
        none = 0,
        warning,
        clipping
    };

    /** */
    ClippingLevel getClippingLevel() const noexcept { return clippingLevel; }

    /** */
    void resetClippingLevel() { clippingLevel = ClippingLevel::none; }

    //==============================================================================
    /** @internal */
    void resized() override;
    /** @internal */
    void paint (Graphics&) override;

private:
    //==============================================================================
    Model* model = nullptr;
    Array<ChannelContext> channels;
    Array<float> levels;
    ClippingLevel clippingLevel = ClippingLevel::none;

    //==============================================================================
    void updateClippingLevel (bool timeToUpdate);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Meter)
};
