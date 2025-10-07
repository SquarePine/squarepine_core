AudioTransportProcessor::AudioTransportProcessor() :
    InternalProcessor (false),
    transport (new AudioTransportSource()),
    source (nullptr)
{
    {
        auto layout = createDefaultParameterLayout();

        auto apb = std::make_unique<AudioParameterBool> (loopingId.toString(), TRANS ("Loop"), false);
        loopingParam = apb.get();
        layout.add (std::move (apb));

        resetAPVTSWithLayout (std::move (layout));
    }

    audioSourceProcessor.setAudioSource (transport, true);
    prepareToPlay (44100.0, 1024);
}

AudioTransportProcessor::~AudioTransportProcessor()
{
    audioSourceProcessor.setAudioSource (nullptr, false);
}

//==============================================================================
void AudioTransportProcessor::play()                            { transport->start(); }
void AudioTransportProcessor::stop()                            { transport->stop(); }
bool AudioTransportProcessor::isLooping() const                 { return loopingParam->get(); }
bool AudioTransportProcessor::isPlaying() const                 { return transport->isPlaying(); }
bool AudioTransportProcessor::hasStreamFinished() const         { return transport->hasStreamFinished(); }
void AudioTransportProcessor::toggleLooping()                   { setLooping (! isLooping()); }
double AudioTransportProcessor::getLengthSeconds() const        { return transport->getLengthInSeconds(); }
int64 AudioTransportProcessor::getLengthSamples() const         { return transport->getTotalLength(); }
double AudioTransportProcessor::getCurrentTimeSeconds() const   { return transport->getCurrentPosition(); }

void AudioTransportProcessor::playFromStart()
{
    setCurrentTime (0.0);
    play();
}

void AudioTransportProcessor::setLooping (const bool shouldLoop)
{
    *loopingParam = shouldLoop;

    transport->setLooping (shouldLoop);

    if (source != nullptr)
        source->setLooping (shouldLoop);
}

int64 AudioTransportProcessor::getCurrentTimeSamples() const
{
    return secondsToSamples (getCurrentTimeSeconds(), getSampleRate());
}

void AudioTransportProcessor::setCurrentTime (const double seconds)
{
    transport->setPosition (seconds);
}

void AudioTransportProcessor::setCurrentTime (const int64 samples)
{
    transport->setPosition (samplesToSeconds (samples, getSampleRate()));
}

void AudioTransportProcessor::clear()
{
    stop();
    transport->setSource (nullptr);
    source.reset();
}

//==============================================================================
void AudioTransportProcessor::setSource (PositionableAudioSource* const s,
                                         bool transportOwnsSource,
                                         const int readAheadBufferSize,
                                         TimeSliceThread* const readAheadThread,
                                         const double sourceSampleRateToCorrectFor,
                                         const int maxNumChannels)
{
    if (source == s)
        return;

    transport->setSource (nullptr);

    source.set (s, transportOwnsSource);
    transport->setSource (source, readAheadBufferSize, readAheadThread,
                          sourceSampleRateToCorrectFor, maxNumChannels);

    prepareToPlay (getSampleRate(), getBlockSize());
    setLooping (isLooping());
}

void AudioTransportProcessor::setSource (AudioFormatReaderSource* const readerSource,
                                         bool transportOwnsSource,
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

    setSource (readerSource, transportOwnsSource, readAheadBufferSize,
               readAheadThread, sampleRate, maxNumChans);
}

void AudioTransportProcessor::setSource (AudioFormatReader* const reader,
                                         const int readAheadBufferSize,
                                         TimeSliceThread* const readAheadThread)
{
    if (reader == nullptr)
    {
        clear();
        return;
    }

    setSource (new AudioFormatReaderSource (reader, true), true,
               readAheadBufferSize, readAheadThread);
}

//==============================================================================
void AudioTransportProcessor::prepareToPlay (const double newSampleRate, const int estimatedSamplesPerBlock)
{
    setRateAndBufferSizeDetails (newSampleRate, estimatedSamplesPerBlock);
    audioSourceProcessor.prepareToPlay (newSampleRate, estimatedSamplesPerBlock);
}

void AudioTransportProcessor::processBlock (juce::AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    buffer.clear();

    if (isSuspended() || isBypassed())
        return;

    audioSourceProcessor.processBlock (buffer, midiMessages);
}

void AudioTransportProcessor::releaseResources()
{
    audioSourceProcessor.releaseResources();
}
