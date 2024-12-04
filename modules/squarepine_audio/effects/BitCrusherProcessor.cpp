BitCrusherProcessor::BitCrusherProcessor()                                          { AudioProcessor::addParameter (bitDepth); }
void BitCrusherProcessor::setBitDepth (int newBitDepth)                             { bitDepth->operator= (newBitDepth); }
int BitCrusherProcessor::getBitDepth() const noexcept                               { return bitDepth->get(); }
void BitCrusherProcessor::processBlock (juce::AudioBuffer<float>& b, MidiBuffer&)   { process (b); }
void BitCrusherProcessor::processBlock (juce::AudioBuffer<double>& b, MidiBuffer&)  { process (b); }

//==============================================================================
template<typename FloatType>
void BitCrusherProcessor::process (juce::AudioBuffer<FloatType>& buffer)
{
    const auto localBitDepth = bitDepth->get();

    if (! isBypassed() && ! buffer.hasBeenCleared())
        for (auto channel : AudioBufferView (buffer))
            for (auto& sample : channel)
                sample = crushSample (sample, localBitDepth);
}
