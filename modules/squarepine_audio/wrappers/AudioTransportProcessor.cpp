AudioTransportProcessor::AudioTransportProcessor() :
    transport (new AudioTransportSource()),
    source (nullptr)
{
    audioSourceProcessor.setAudioSource (transport, true);
    prepareToPlay (44100.0, 256);
}

AudioTransportProcessor::~AudioTransportProcessor()
{
    audioSourceProcessor.setAudioSource (nullptr, false);
}

//==============================================================================
const String AudioTransportProcessor::getName() const
{
    return TRANS ("Audio Transport");
}

Identifier AudioTransportProcessor::getIdentifier() const
{
    return "AudioTransportProcessor";
}

//==============================================================================
void AudioTransportProcessor::play()
{
    const ScopedLock lock (getCallbackLock());
    transport->start();
}

void AudioTransportProcessor::playFromStart()
{
    const ScopedLock lock (getCallbackLock());
    transport->setPosition (0);
    transport->start();
}

void AudioTransportProcessor::stop()
{
    const ScopedLock lock (getCallbackLock());
    transport->stop();
}

void AudioTransportProcessor::setLooping (const bool shouldLoop)
{
    const ScopedLock lock (getCallbackLock());

    transport->setLooping (shouldLoop);

    if (source != nullptr)
        source->setLooping (shouldLoop);
}

bool AudioTransportProcessor::isLooping() const
{
    return transport->isLooping();
}

bool AudioTransportProcessor::isPlaying() const
{
    return transport->isPlaying();
}

double AudioTransportProcessor::getLengthSeconds() const noexcept
{
    return transport->getLengthInSeconds();
}

int64 AudioTransportProcessor::getLengthSamples() const noexcept
{
    return transport->getTotalLength();
}

double AudioTransportProcessor::getCurrentTimeSeconds() const noexcept
{
    return transport->getCurrentPosition();
}

int64 AudioTransportProcessor::getCurrentTimeSamples() const noexcept
{
    return (int64) (getCurrentTimeSeconds() * getSampleRate());
}

void AudioTransportProcessor::setCurrentTime (const double newPosition)
{
    transport->setPosition (newPosition);
}

void AudioTransportProcessor::setCurrentTime (const int64 samples)
{
    transport->setPosition ((double) samples / getSampleRate());
}

void AudioTransportProcessor::setResamplingRatio (const double)
{
}

void AudioTransportProcessor::clear()
{
    const ScopedLock lock (getCallbackLock());
    stop();
    transport->setSource (nullptr);
    source = nullptr;
}

//==============================================================================
void AudioTransportProcessor::setSource (PositionableAudioSource* const s,
                                         const int readAheadBufferSize,
                                         TimeSliceThread* const readAheadThread,
                                         const double sourceSampleRateToCorrectFor,
                                         const int maxNumChannels)
{
    const ScopedLock lock (getCallbackLock());

    source = s;
    transport->setSource (source, readAheadBufferSize, readAheadThread,
                          sourceSampleRateToCorrectFor, maxNumChannels);

    prepareToPlay (getSampleRate(), getBlockSize());
}

void AudioTransportProcessor::setSource (AudioFormatReaderSource* const readerSource,
                                         const int readAheadBufferSize,
                                         TimeSliceThread* const readAheadThread)
{
    int maxNumChans = 2;
    auto sampleRate = 44100.0;

    if (readerSource != nullptr)
    {
        if (auto* afr = readerSource->getAudioFormatReader())
        {
            maxNumChans = (int) afr->numChannels;
            sampleRate = afr->sampleRate;
        }
    }

    setSource (readerSource, readAheadBufferSize, readAheadThread, sampleRate, maxNumChans);
}

//==============================================================================

void AudioTransportProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);

    audioSourceProcessor.prepareToPlay (newSampleRate, estimatedSamplesPerBlock);
}

void AudioTransportProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    audioSourceProcessor.processBlock (buffer, midiMessages);
}

void AudioTransportProcessor::releaseResources()
{
    audioSourceProcessor.releaseResources();
}
