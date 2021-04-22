/** The AudioBufferFIFO implements an actual sample buffer using AbstractFIFO.

    You can add samples from the various kind of formats,
    like float pointers or AudioBuffers.

    Then you can read into float arrays, juce::AudioBuffers or even AudioSourceChannelInf
    to be used directly in AudioSources.
*/
template<typename FloatType>
class AudioBufferFIFO final : public AbstractFifo
{
public:
    /** Creates a FIFO with a buffer of given number of channels and samples. */
    AudioBufferFIFO (int channels = 2, int buffersize = 8192) :
        AbstractFifo (buffersize)
    {
        setSize (channels, buffersize);
    }

    /** Destructor. */
    virtual ~AudioBufferFIFO() = default;

    //==============================================================================
    /** @returns the number of channels of the underlying buffer. */
    int getNumChannels() const noexcept { return buffer.getNumChannels(); }

    //==============================================================================
    /** Resize the buffer with new number of channels and new number of samples. */
    void setSize (int channels, int newBufferSize)
    {
        buffer.setSize (channels, newBufferSize, false, true, true);
        setTotalSize (newBufferSize);
    }

    //==============================================================================
    /** Push samples into the FIFO from raw float arrays. */
    void push (const FloatType** samples, int numSamples)
    {
        numSamples = jmin (getFreeSpace(), numSamples);
        if (numSamples <= 0)
            return;

        int start1, size1, start2, size2;
        prepareToWrite (numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                buffer.copyFrom (channel, start1, samples[channel], size1);

        if (size2 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                buffer.copyFrom (channel, start2, samples[channel] + size1, size2);

        finishedWrite (size1 + size2);
    }

    /** Push samples into the FIFO from an AudioBuffer. */
    void push (const juce::AudioBuffer<FloatType>& samples, int numSamples = -1)
    {
        push (samples.getArrayOfReadPointers(),
              numSamples < 0 ? samples.getNumSamples() : numSamples);
    }

    //==============================================================================
    /** Add silence to the FIFO. */
    void pushSilence (int numSamples)
    {
        numSamples = jmin (getFreeSpace(), numSamples);
        if (numSamples <= 0)
            return;

        int start1, size1, start2, size2;
        prepareToWrite (numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            buffer.clear (start1, size1);

        if (size2 > 0)
            buffer.clear (start2, size2);

        finishedWrite (size1 + size2);
    }

    //==============================================================================
    /** Read samples from the FIFO into raw float arrays. */
    void readTo (FloatType** samples, int numSamples)
    {
        numSamples = jmin (getNumReady(), numSamples);
        if (numSamples <= 0)
            return;

        int start1, size1, start2, size2;
        prepareToRead (numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                FloatVectorOperations::copy (samples[channel],
                                             buffer.getReadPointer (channel, start1),
                                             size1);

        if (size2 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                FloatVectorOperations::copy (samples[channel] + size1,
                                             buffer.getReadPointer (channel, start2),
                                             size2);

        finishedRead (size1 + size2);
    }

    /** Read samples from the FIFO into an AudioBuffer. */
    void readTo (juce::AudioBuffer<FloatType>& samples, int numSamples = -1)
    {
        readTo (samples.getArrayOfWritePointers(),
                numSamples < 0 ? samples.getNumSamples() : numSamples);
    }

    /** Read samples from the FIFO into AudioSourceChannelInfo buffers to be used in AudioSources getNextAudioBlock */
    void readTo (const AudioSourceChannelInfo &info, int numSamples = -1)
    {
        if (auto* buff = info.buffer)
            readTo (*buff, numSamples);
    }

    //==============================================================================
    /** Read samples from the FIFO and add it to raw float arrays. */
    void readToAdding (FloatType** samples, int numSamples, FloatType gain = FloatType (1))
    {
        numSamples = jmin (getNumReady(), numSamples);
        if (numSamples <= 0)
            return;

        int start1, size1, start2, size2;
        prepareToRead (numSamples, start1, size1, start2, size2);

        if (size1 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                FloatVectorOperations::addWithMultiply (samples[channel],
                                                        buffer.getReadPointer (channel, start1),
                                                        gain,
                                                        size1);

        if (size2 > 0)
            for (int channel = 0; channel < getNumChannels(); ++channel)
                FloatVectorOperations::addWithMultiply (samples[channel] + size1,
                                                        buffer.getReadPointer (channel, start2),
                                                        gain,
                                                        size2);

        finishedRead (size1 + size2);
    }

    /** Read samples from the FIFO adding it to the AudioBuffers */
    void readToAdding (juce::AudioBuffer<FloatType>& samples, int numSamples = -1, FloatType gain = FloatType (1))
    {
        readFromFifoAdding (samples.getArrayOfWritePointers(),
                            numSamples < 0 ? samples.getNumSamples() : numSamples,
                            gain);
    }

    /** Read samples from the FIFO into AudioSourceChannelInfo buffers to be used in AudioSources getNextAudioBlock */
    void readToAdding (const AudioSourceChannelInfo& info,
                             int numSamples = -1,
                             FloatType gain = FloatType (1))
    {
        if (auto* buff = info.buffer)
            readFromFifoAdding (*buff, numSamples, gain);
    }

    //==============================================================================
    /** Pop (consume/remove) samples from the FIFO, ignoring them. */
    void pop (int numSamples)
    {
        numSamples = jmin (getNumReady(), numSamples);
        if (numSamples <= 0)
            return;

        int start1, size1, start2, size2;
        prepareToRead (numSamples, start1, size1, start2, size2);
        finishedRead (size1 + size2);
    }

    /** Clears all samples and sets the FIFO state to empty. */
    void clear()
    {
        buffer.clear();
        reset();
    }

private:
    /** The actual audio buffer */
    juce::AudioBuffer<FloatType> buffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioBufferFIFO)
};

