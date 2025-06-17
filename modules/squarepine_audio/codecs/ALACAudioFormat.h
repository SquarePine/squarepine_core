/** Reads and writes the ALAC audio format.

    @see AudioFormat

    @tags{Audio}
*/
class ALACAudioFormat final : public AudioFormat
{
public:
    /** Constructor. */
    ALACAudioFormat();

    //==============================================================================
    /** @internal */
    Array<int> getPossibleSampleRates() override;
    /** @internal */
    Array<int> getPossibleBitDepths() override;
    /** @internal */
    bool canDoStereo() override { return true; }
    /** @internal */
    bool canDoMono() override { return true; }
    /** @internal */
    bool isCompressed() override { return true; }
    /** @internal */
    AudioFormatReader* createReaderFor (InputStream*, bool) override;
    /** @internal */
    std::unique_ptr<AudioFormatWriter> createWriterFor (std::unique_ptr<OutputStream>&, const AudioFormatWriterOptions&) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ALACAudioFormat)
};
