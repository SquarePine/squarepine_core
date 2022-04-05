//==============================================================================
#if SQUAREPINE_USE_ELASTIQUE

    /** You have direct access to the audio file, right?
        Elastique definitely needs to read a chunk of samples in advance which would produce some latency
        if looking at it like a realtime process, BUT as you have access to the input audio you don't
        have to read the audio in sync to the output audio (this doesn't work in a time stretch
        context anyway since in and out buffers are not in sync).

        The (normal) elastique interface works like a callback for and audio device.
        Usually the audio device has a fixed output blocksize, so each time it's
        called it requests this blocksize (say 512 audio frames).

        Elastique works the same way: you initialize it for a blocksize, of 512 for example,
        and it tells you how much data it needs to produce this, like this:

        1. call: audio call back requests 512 samples,
                Elastique needs 4096 to produce this,
                so you read 4096 samples from the input file,
                feed it into elastique and it returns 512 to pass on to the audio callback
        2. call: audio call back requests 512 samples,
                Elastique needs 0 to produce this, so you read nothing,
                call the process function of elastique anyway
                and it return 512 samples to pass on to the audio callback
        3. call: audio call back requests 512 samples,
                Elastique needs 512 to produce this,
                so you read 512 samples from the input file,
                feed it into elastique and it returns 512 to pass on to the audio callback
        etc...

        If you don't want direct audio access in the audio callback
        you need to implement the FIFO thing that's being filled by a separate thread.

        If you have changing blocksizes you can change this for elastique
        by passing the new blocksize as argument via the GetFramesNeeded method.

        ---

        Something around 512 block sizes is a good size.
        If you're lookin at much smaller blocksizes (32-64)
        you should look into the DirectAPI

        What are the key differences between Direct and vanilla?
        Better performance control - you can split up the processing -
        but more buffering (and brain) work on your side.
    */
    namespace zplane
    {
        //==============================================================================
        /** */
        struct ElastiqueDeleter final
        {
            void operator() (CElastiqueProV3If* p)
            {
                isValid (CElastiqueProV3If::DestroyInstance (p));
            }

            void operator() (CElastiqueProV3DirectIf* p)
            {
                isValid (CElastiqueProV3DirectIf::DestroyInstance (p));
            }
        };

        //==============================================================================
        /** */
        using ElastiquePtr = std::unique_ptr<CElastiqueProV3If, ElastiqueDeleter>;
        /** */
        using ElastiqueDirectPtr = std::unique_ptr<CElastiqueProV3DirectIf, ElastiqueDeleter>;

        //==============================================================================
        /** */
        inline ElastiquePtr createElastiquePtr (int bufferSize,
                                                int numChannels,
                                                double inputSampleRate,
                                                CElastiqueProV3If::ElastiqueMode_t mode = CElastiqueProV3If::kV3Eff,
                                                double minCombinedFactor = 0.1)
        {
            if (bufferSize > 1024)
            {
                // CElastiqueProV3If works with a strict maximum of 1024 samples.
                // This is hard-coded under their hood and will puke if trying to be configured otherwise!
                jassertfalse;
                return {};
            }

            jassert (bufferSize > 0);

            CElastiqueProV3If* result = nullptr;
            const auto code = CElastiqueProV3If::CreateInstance (result, bufferSize, numChannels,
                                                                 (float) inputSampleRate, mode,
                                                                 (float) minCombinedFactor);
            if (isValid (code))
            {
                result->SetStereoInputMode (CElastiqueProV3If::kPlainStereoMode);
                return ElastiquePtr (result);
            }

            return {};
        }

        /** */
        inline ElastiqueDirectPtr createElastiqueDirectPtr (int numChannels,
                                                            double inputSampleRate,
                                                            CElastiqueProV3DirectIf::ElastiqueVersion_t mode = CElastiqueProV3DirectIf::kV3Eff,
                                                            double minCombinedFactor = 0.1)
        {
            CElastiqueProV3DirectIf* result = nullptr;
            const auto code = CElastiqueProV3DirectIf::CreateInstance (result, numChannels,
                                                                       (float) inputSampleRate, mode,
                                                                       (float) minCombinedFactor);
            if (isValid (code))
            {
                result->SetStereoInputMode (CElastiqueProV3DirectIf::kPlainStereoMode);
                return ElastiqueDirectPtr (result);
            }

            return {};
        }

        //==============================================================================
        inline void logElastiqueInfo()
        {
            auto newLine = juce::newLine;

            juce::String info;
            info
            << newLine
            << "--------------------------------------------------" << newLine << newLine
            << "=== Elastique Information ===" << newLine << newLine
            << "Build Date: " << CElastiqueProV3If::GetBuildDate() << newLine
            << "Version: " << CElastiqueProV3If::GetVersion () << newLine
            << newLine
            << "--------------------------------------------------" << newLine;

            juce::Logger::writeToLog (info);
        }
    }

#endif
