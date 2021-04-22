//==============================================================================
/** */
template<typename FloatType>
class AudioChannelView final
{
public:
    //==============================================================================
    /** */
    class SampleIterator final
    {
    public:
        /** */
        SampleIterator (FloatType* channel_) noexcept :
            channel (channel_)
        {
        }

        /** */
        SampleIterator (const SampleIterator& other) noexcept :
            channel (other.channel)
        {
        }

        /** */
        SampleIterator operator= (const SampleIterator& other) noexcept
        {
            channel = other.channel;
            return *this;
        }

        /** */
        FloatType* operator++()                                         { channel++; return channel; }
        /** */
        FloatType& operator*()                                          { return *channel; }
        /** */
        const FloatType& operator*() const                              { return *channel; }
        /** */
        bool operator== (const SampleIterator& other) const noexcept    { return channel == other.channel; }
        /** */
        bool operator!= (const SampleIterator& other) const noexcept    { return channel != other.channel; }

    private:
        FloatType* channel = nullptr;
    };

    //==============================================================================
    /** */
    AudioChannelView (FloatType* channel_, int numSamples_) noexcept :
        channel (channel_),
        numSamples (numSamples_)
    {
    }

    /** */
    AudioChannelView (const AudioChannelView& other) noexcept :
        channel (other.channel),
        numSamples (other.numSamples)
    {
    }

    /** */
    AudioChannelView operator= (const AudioChannelView& other) noexcept
    {
        channel = other.channel;
        numSamples = other.numSamples;
        return *this;
    }

    /** */
    FloatType& operator() (int index)               { return channel[index]; }
    /** */
    const FloatType& operator() (int index) const   { return channel[index]; }
    /** */
    SampleIterator begin() noexcept                 { return SampleIterator (&channel[0]); }
    /** */
    SampleIterator end() noexcept                   { return SampleIterator (&channel[numSamples]); }
    /** */
    const SampleIterator cbegin() const noexcept    { return SampleIterator (&channel[0]); }
    /** */
    const SampleIterator cend() const noexcept      { return SampleIterator (&channel[numSamples]); }

private:
    FloatType* channel = nullptr;
    int numSamples = 0;
};

//==============================================================================
/** */
template<typename FloatType>
class AudioBufferView final
{
public:
    //==============================================================================
    /** */
    class AudioChannelIterator final
    {
    public:
        /** */
        AudioChannelIterator (FloatType** channels_, int numSamples_) noexcept :
            channel (channels_),
            numSamples (numSamples_)
        {
        }

        /** */
        AudioChannelIterator (const AudioChannelIterator& other) noexcept :
            channel (other.channel),
            numSamples (other.numSamples)
        {
        }

        /** */
        AudioChannelIterator operator= (const AudioChannelIterator& other) noexcept
        {
            channel = other.channel;
            numSamples = other.numSamples;
            return *this;
        }

        /** */
        FloatType** operator++()                    { channel++; return channel; }
        /** */
        AudioChannelView<FloatType> operator*()     { return AudioChannelView<FloatType> (*channel, numSamples); }

        /** */
        bool operator== (const AudioChannelIterator& other) const noexcept { return channel == other.channel; }
        /** */
        bool operator!= (const AudioChannelIterator& other) const noexcept { return channel != other.channel; }

    private:
        FloatType** channel = nullptr;
        int numSamples = 0;
    };

    //==============================================================================
    /** */
    AudioBufferView (FloatType** channels_, int numChannels_, int numSamples_) noexcept :
        channels (channels_),
        numChannels (numChannels_),
        numSamples (numSamples_)
    {
    }

    /** */
    AudioBufferView (juce::AudioBuffer<FloatType>& buffer) noexcept :
        channels (buffer.getArrayOfWritePointers()),
        numChannels (buffer.getNumChannels()),
        numSamples (buffer.getNumSamples())
    {
    }

    /** */
    AudioBufferView (const AudioBufferView& other) noexcept :
        channels (other.channels),
        numChannels (other.numChannels),
        numSamples (other.numSamples)
    {
    }

    /** */
    AudioBufferView operator= (const AudioBufferView& other) noexcept
    {
        channels = other.channels;
        numChannels = other.numChannels;
        numSamples = other.numSamples;
        return *this;
    }

    /** */
    FloatType& operator() (int chan, int samp)              { return channels[chan][samp]; }
    /** */
    const FloatType& operator() (int chan, int samp) const  { return channels[chan][samp]; }
    /** */
    AudioChannelIterator begin()                            { return AudioChannelIterator (&channels[0], numSamples); }
    /** */
    AudioChannelIterator end()                              { return AudioChannelIterator (&channels[numChannels], numSamples); }
    /** */
    const AudioChannelIterator begin() const noexcept       { return AudioChannelIterator (&channels[0], numSamples); }
    /** */
    const AudioChannelIterator end() const noexcept         { return AudioChannelIterator (&channels[numChannels], numSamples); }

private:
    FloatType** channels = nullptr;
    int numChannels = 0, numSamples = 0;
};
