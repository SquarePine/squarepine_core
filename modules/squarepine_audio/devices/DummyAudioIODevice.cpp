DummyAudioIODevice::DummyAudioIODevice (const bool isInput,
                                        const int chans,
                                        const double sr,
                                        const int numSamples) :
    AudioIODevice ("Dummy " + String (isInput ? "Input" : "Output"), "Dummy"),
    Thread ("Dummy Audio"),
    numChannels (chans),
    bufferSize (numSamples),
    sampleRate (sr)
{
    jassert (bufferSize > 0);
    jassert (numChannels > 0);
    jassert (sampleRate > 0.0);
}

DummyAudioIODevice::~DummyAudioIODevice()
{
    close();

    stopThread (3000);
}

//==============================================================================
StringArray DummyAudioIODevice::getOutputChannelNames()
{
    StringArray chans;

    for (int i = 0; i < getNumChannels(); ++i)
        chans.add ("Output Channel " + (i < 9 ? "0" + String (i + 1) : String (i + 1)));

    return chans;
}

StringArray DummyAudioIODevice::getInputChannelNames()
{
    StringArray chans;

    for (int i = 0; i < getNumChannels(); ++i)
        chans.add ("Input Channel " + (i < 10 ? "0" + String (i + 1) : String (i + 1)));

    return chans;
}

Array<double> DummyAudioIODevice::getAvailableSampleRates() { return { 44100.0, 48000.0, 88200.0, 96000.0, 192000.0 }; }
Array<int> DummyAudioIODevice::getAvailableBufferSizes()    { return { 64, 512, 1024, 2048, 4096, 8192 }; }
int DummyAudioIODevice::getDefaultBufferSize()              { return 1024; }
bool DummyAudioIODevice::isPlaying()                        { return playing; }
bool DummyAudioIODevice::isOpen()                           { return opened; }
String DummyAudioIODevice::getLastError()                   { return {}; }
int DummyAudioIODevice::getCurrentBufferSizeSamples()       { return bufferSize; }
double DummyAudioIODevice::getCurrentSampleRate()           { return sampleRate; }
int DummyAudioIODevice::getCurrentBitDepth()                { return 32; }
int DummyAudioIODevice::getNumChannels() const              { return numChannels; }
int DummyAudioIODevice::getOutputLatencyInSamples()         { return 0; }
int DummyAudioIODevice::getInputLatencyInSamples()          { return 0; }

//==============================================================================
String DummyAudioIODevice::open (const int numOutputChannels,
                                 const double sr,
                                 const int bufferSizeSamples)
{
    close();

    opened = true;
    setCurrentBufferSizeSamples (bufferSizeSamples);
    setCurrentSampleRate (sr);
    setNumChannels (numOutputChannels);

    return {};
}

String DummyAudioIODevice::open (const BigInteger& inputChannels,
                                 const BigInteger& outputChannels,
                                 const double sr,
                                 const int bufferSizeSamples)
{
    return open (jmax (inputChannels.countNumberOfSetBits(), outputChannels.countNumberOfSetBits()), sr, bufferSizeSamples);
}

void DummyAudioIODevice::close()
{
    stop();

    if (opened)
    {
        opened = false;
        playing = false;
    }
}

void DummyAudioIODevice::start (AudioIODeviceCallback* cb)
{
    callback = cb;

    if (callback != nullptr)
    {
        callback->audioDeviceAboutToStart (this);
        startThread (10);
        playing = true;
    }
}

void DummyAudioIODevice::stop()
{
    signalThreadShouldExit();
    waitForThreadToExit (3000);

    playing = false;

    auto* const lastCallback = callback;

    {
        const ScopedLock sl (callbackLock);
        callback = nullptr;
    }

    if (lastCallback != nullptr)
        lastCallback->audioDeviceStopped();
}

//==============================================================================
void DummyAudioIODevice::setCurrentBufferSizeSamples (const int size)
{
    const auto sizes = getAvailableBufferSizes();
    const auto it = std::min_element (sizes.begin(), sizes.end());
    jassert (it != sizes.end());

    bufferSize = jmax (*it, size);
}

void DummyAudioIODevice::setCurrentSampleRate (const double sr)
{
    const auto rates = getAvailableSampleRates();
    const auto it = std::min_element (rates.begin(), rates.end());
    jassert (it != rates.end());

    sampleRate = jmax (*it, sr);
}

void DummyAudioIODevice::setNumChannels (const int numChans)
{
    numChannels = jmax (2, numChans);
}

//==============================================================================
BigInteger DummyAudioIODevice::getActiveInputChannels() const
{
    BigInteger chan;

    for (int i = 0; i < numChannels; ++i)
        chan.setBit (i);

    return chan; // All channels are available.
}

BigInteger DummyAudioIODevice::getActiveOutputChannels() const
{
    return getActiveInputChannels(); // All channels are available.
}

void DummyAudioIODevice::run()
{
    auto lastRenderTimeTicks = Time::getHighResolutionTicks();

    while (! threadShouldExit())
    {
        const auto numSamples = bufferSize.load();

        {
            const ScopedLock sl (callbackLock);
            if (callback != nullptr)
            {
                const auto numChans = numChannels.load();

                juce::AudioBuffer<float> input (numChans, numSamples);
                input.clear();

                juce::AudioBuffer<float> output (numChans, numSamples);
                output.clear();
                callback->audioDeviceIOCallbackWithContext (input.getArrayOfReadPointers(), numChans,
                                                            output.getArrayOfWritePointers(), numChans,
                                                            numSamples, {});
            }
        }

        const auto sleepTime = Time::secondsToHighResolutionTicks (timeSamplesToSeconds (numSamples, sampleRate));
        lastRenderTimeTicks += sleepTime;
        waitUntilTime (lastRenderTimeTicks, 1);
    }
}
