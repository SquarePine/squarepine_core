ResamplingAudioFormatReader::ResamplingAudioFormatReader (std::shared_ptr<AudioFormatReader> formatReader,
                                                          std::shared_ptr<TimeSliceThread> tst) :
    AudioFormatReader (formatReader->input, formatReader->getFormatName() + "-SRC"),
    reader (formatReader),
    timeSliceThread (tst)
{
    jassert (reader != nullptr);

    sampleRate = originalSampleRate = reader->sampleRate;
    jassert (sampleRate != 0.0);

    bitsPerSample = reader->bitsPerSample;
    lengthInSamples = reader->lengthInSamples;
    numChannels = reader->numChannels;
    usesFloatingPointData = reader->usesFloatingPointData;
    metadataValues = reader->metadataValues;
    input = nullptr;
}

ResamplingAudioFormatReader::ResamplingAudioFormatReader (std::shared_ptr<AudioFormatReader> formatReader,
                                                          int expectedReadBlockSize, double outputSampleRate,
                                                          std::shared_ptr<TimeSliceThread> tst) :
    ResamplingAudioFormatReader (formatReader, tst)
{
    prepare (outputSampleRate, expectedReadBlockSize);
}

ResamplingAudioFormatReader::~ResamplingAudioFormatReader()
{
    input = nullptr;    // Prevent the base-class from deleting the input...
    source = nullptr;   // Clear before the time-slice thread...
}

//==============================================================================
void ResamplingAudioFormatReader::prepare (double currentOutputSampleRate, int expectedReadBlockSize)
{
    if (approximatelyEqual (originalSampleRate, 0.0) || approximatelyEqual (currentOutputSampleRate, 0.0))
    {
        jassertfalse;
        return;
    }

    sourceRatio = currentOutputSampleRate / originalSampleRate;

    // We're in a resampling situation, and the block size has changed, probably due to time-stretching
    const bool blockSizeChanged = approximatelyEqual (sampleRate, currentOutputSampleRate)
                               && currentOutputSampleRate != originalSampleRate
                               && expectedReadBlockSize != sourceBlockSize;

    // We're currently using a sample rate that's different than the current sample rate used by the interface
    if (! approximatelyEqual (sampleRate, currentOutputSampleRate) || blockSizeChanged)
    {
        sampleRate = currentOutputSampleRate;
        lengthInSamples = std::llround (static_cast<double> (reader->lengthInSamples) * sourceRatio);

        if (sampleRate != originalSampleRate || expectedReadBlockSize != sourceBlockSize)
        {
            if (source == nullptr)
                source.reset (new AudioTransportSource());

            sourceBlockSize = expectedReadBlockSize;
            source->prepareToPlay (sourceBlockSize, currentOutputSampleRate);

            if (rSource == nullptr)
                rSource.reset (new AudioFormatReaderSource (reader.get(), false));

            const auto readAheadSize = timeSliceThread == nullptr ? 0 : 2048;

            source->setSource (rSource.get(), readAheadSize, timeSliceThread.get(), reader->sampleRate);

            outputBuffer.setSize ((int) numChannels, sourceBlockSize, false, true, true);
        }
    }
}

//==============================================================================
bool ResamplingAudioFormatReader::readSamples (int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer,
                                               int64 startSampleInFile, int numSamples)
{
    jassert (reader != nullptr && ! approximatelyEqual (sampleRate, 0.0));
    if (reader == nullptr)
        return false;

    // Pass through if no SRC required
    if (approximatelyEqual (sourceRatio, 1.0))
        return reader->readSamples (destSamples, numDestChannels, startOffsetInDestBuffer, startSampleInFile, numSamples);

    if (source != nullptr)
    {
        if (outputBuffer.getNumSamples() != numSamples)
            outputBuffer.setSize (numDestChannels, numSamples);

        if (! source->isPlaying())
            source->start();

        /** To avoid playback issues when moving from block to block, "source" manages its position
            The interface to this function specifies a playback position, so we check to see whether that new position
            is different to the "source" position (within a tolerance) if it is then we move the source's position to the requested position
        */
        const auto newPosition = std::llround ((double) startSampleInFile * (1.0 / sourceRatio));
        const auto diff = std::abs (rSource->getNextReadPosition() - newPosition);

        constexpr auto positionDiff = 5;
        if (diff > positionDiff)
            rSource->setNextReadPosition (newPosition);

        AudioSourceChannelInfo info (&outputBuffer, 0, numSamples);
        source->getNextAudioBlock (info);
    }

    writeOutputBuffers (destSamples, numDestChannels, startOffsetInDestBuffer, numSamples);

    return true;
}

//==============================================================================
void ResamplingAudioFormatReader::writeOutputBuffers (int* const* destSamples, int numDestChannels, int startOffsetInDestBuffer, int numSamples)
{
    const auto localNumChannels = (int) numChannels;
    const bool localUsesFloatingPointData = usesFloatingPointData;

    // Copy SRC-ed samples to output
    if (localUsesFloatingPointData)
    {
        const auto offset = (size_t) startOffsetInDestBuffer;

        for (int i = numDestChannels; --i >= 0;)
        {
            if (auto* targetChannel = destSamples[i])
            {
                const float* sourceChannel = nullptr;

                if (i < localNumChannels)
                    sourceChannel = outputBuffer.getReadPointer (i, 0);

                if (sourceChannel != nullptr)
                    FloatVectorOperations::copy (reinterpret_cast<float*> (targetChannel) + offset, sourceChannel, numSamples);
                else
                    FloatVectorOperations::clear (reinterpret_cast<float*> (targetChannel) + offset, numSamples);
            }
        }
    }
    else
    {
        // Convert from float to int32:
        using DestinationType = AudioData::Pointer<AudioData::Int32, AudioData::LittleEndian, AudioData::NonInterleaved, AudioData::NonConst>;
        using SourceType = AudioData::Pointer<AudioData::Float32, AudioData::LittleEndian, AudioData::NonInterleaved, AudioData::Const>;

        for (int i = numDestChannels; --i >= 0;)
        {
            if (auto* targetChannel = destSamples[i])
            {
                DestinationType dest (targetChannel);
                dest += startOffsetInDestBuffer;

                const float* sourceChannel = nullptr;

                if (i < localNumChannels)
                    sourceChannel = outputBuffer.getReadPointer (i, 0);

                if (sourceChannel != nullptr)
                    dest.convertSamples (SourceType (sourceChannel), numSamples);
                else
                    dest.clearSamples (numSamples);
            }
        }
    }
}
