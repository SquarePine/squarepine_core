//==============================================================================
#if SQUAREPINE_USE_AUFTAKT

    namespace zplane
    {
        //==============================================================================
        /** */
        struct AuftaktDeleter final
        {
            void operator() (CaufTAKT_If* p)
            {
                isValid (CaufTAKT_If::DestroyInstance (p));
            }
        };

        //==============================================================================
        /** */
        using AuftaktPtr = std::unique_ptr<CaufTAKT_If, AuftaktDeleter>;

        //==============================================================================
        /** */
        inline AuftaktPtr createAuftaktPtr (int numChannels,
                                            double inputSampleRate,
                                            double downbeatBlockSizeInSeconds = 30.0,
                                            CaufTAKT_If::eaufTAKTVersion_t version = CaufTAKT_If::kaufTAKT3)
        {
            CaufTAKT_If* result = nullptr;
            const auto code = CaufTAKT_If::CreateInstance (result, (int) inputSampleRate, numChannels,
                                                           version, (float) downbeatBlockSizeInSeconds);
            if (isValid (code))
                return AuftaktPtr (result);

            return {};
        }

        //==============================================================================
        inline void logAuftaktInfo()
        {
            auto newLine = juce::newLine;

            juce::String info;
            info
            << newLine
            << "--------------------------------------------------" << newLine << newLine
            << "=== Auftakt Information ===" << newLine << newLine
            << "Build Date: " << CaufTAKT_If::GetBuildDate() << newLine
            << "Version - Major: " << CaufTAKT_If::GetVersion (CaufTAKT_If::kMajor) << newLine
            << "Version - Minor: " << CaufTAKT_If::GetVersion (CaufTAKT_If::kMinor) << newLine
            << "Version - Patch: " << CaufTAKT_If::GetVersion (CaufTAKT_If::kPatch) << newLine
            << "Version - Build: " << CaufTAKT_If::GetVersion (CaufTAKT_If::kBuild) << newLine
            << newLine
            << "--------------------------------------------------" << newLine;

            juce::Logger::writeToLog (info);
        }
    }

#endif
