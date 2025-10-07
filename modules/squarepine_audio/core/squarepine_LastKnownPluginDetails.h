/** Use this to populate a fixed set of details that represent an AudioProcessor's set of details.

    This can help properly configure a processor during prepareToPlay, or avoid doing so redundantly.
*/
struct LastKnownPluginDetails final
{
    /** Constructs an empty set of details. */
    LastKnownPluginDetails() noexcept = default;
    /** Makes a copy of a set of details. */
    LastKnownPluginDetails (const LastKnownPluginDetails&) noexcept = default;
    /** Destructor. */
    ~LastKnownPluginDetails() noexcept = default;
    /** Makes a copy of a set of details. */
    LastKnownPluginDetails& operator= (const LastKnownPluginDetails&) noexcept = default;

    /** Creates a set of plugin details from a provided AudioProcessor. */
    LastKnownPluginDetails (AudioProcessor& processor) noexcept :
        sampleRate (processor.getSampleRate()),
        blockSize (processor.getBlockSize()),
        numInputBuses (processor.getBusCount (true)),
        numOutputBuses (processor.getBusCount (false)),
        numInputMainBusChannels (processor.getMainBusNumInputChannels()),
        numOutputMainBusChannels (processor.getMainBusNumOutputChannels()),
        totalNumInputChannels (processor.getTotalNumInputChannels()),
        totalNumOutputChannels (processor.getTotalNumOutputChannels())
    {
    }

    //==============================================================================
    /** @returns true if the details match those provided. */
    [[nodiscard]] bool operator== (const LastKnownPluginDetails& other) const noexcept
    {
        return approximatelyEqual (sampleRate, other.sampleRate)
            && blockSize == other.blockSize
            && numInputBuses == other.numInputBuses
            && numOutputBuses == other.numOutputBuses
            && numInputMainBusChannels == other.numInputMainBusChannels
            && numOutputMainBusChannels == other.numOutputMainBusChannels
            && totalNumInputChannels == other.totalNumInputChannels
            && totalNumOutputChannels == other.totalNumOutputChannels;
    }

    /** @returns true if these details don't match the provided details. */
    [[nodiscard]] bool operator!= (const LastKnownPluginDetails& other) const noexcept
    {
        return ! operator== (other);
    }

    //==============================================================================
    /** @returns a string containing all of the details.

        This is mostly intended for logging purposes.
    */
    [[nodiscard]] String toString() const
    {
        String info;
        info.preallocateBytes (320);

        info
            << "Sample Rate: " << sampleRate << newLine
            << "Block Size: " << blockSize << newLine
            << "Num input busses detected: " << numInputBuses << newLine
            << "Num output busses detected: " << numOutputBuses << newLine
            << "Main buss num input channels detected: " << numInputMainBusChannels << newLine
            << "Main buss num output channels detected: " << numOutputMainBusChannels << newLine
            << "Total num input channels detected: " << totalNumInputChannels << newLine
            << "Total num output channels detected: " << totalNumOutputChannels << newLine;

        return info;
    }

    //==============================================================================
    double sampleRate = 0.0;            // Indicates the last known sample rate.
    int blockSize = 0,                  // Indicates the last known buffer or block size, in samples.
        numInputBuses = 0,              // Indicates the last known number of input buses.
        numOutputBuses = 0,             // Indicates the last known number of output buses.
        numInputMainBusChannels = 0,    // Indicates the last known number of main input bus channels.
        numOutputMainBusChannels = 0,   // Indicates the last known number of main output bus channels.
        totalNumInputChannels = 0,      // Indicates the last known total number of input channels.
        totalNumOutputChannels = 0;     // Indicates the last known total number of output channels.
};
