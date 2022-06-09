//==============================================================================
#if ! (JUCE_WINDOWS || JUCE_MAC) || JUCE_MINGW
    #undef SQUAREPINE_USE_CUESDK
#endif

//==============================================================================
#if SQUAREPINE_USE_CUESDK

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Weverything", "-Wall", "-Wpedantic", "-Wno-eof-newline")

    namespace corsair
    {
        #undef CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS
        #define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS 1
        #include "../../../../sdks/CUESDK/include/CUESDK.h"

        //==============================================================================
        /** @returns an 'ok' result on sucess and a 'fail' result otherwise. */
        inline juce::Result toResult (CorsairError code)
        {
            switch (code)
            {
                case CE_Success:                    return juce::Result::ok();
                case CE_ServerNotFound:             return juce::Result::fail ("Server Not Found");
                case CE_NoControl:                  return juce::Result::fail ("No Control");
                case CE_ProtocolHandshakeMissing:   return juce::Result::fail ("Protocol Handshake Missing");
                case CE_IncompatibleProtocol:       return juce::Result::fail ("Incompatible Protocol");
                case CE_InvalidArguments:           return juce::Result::fail ("Invalid Arguments");
                default:                            break;
            }

            return juce::Result::fail ("Unknown Error");
        }

        /** @returns true if the result code is a success/non-error.
            Logs the error code and asserts on failure.
        */
        inline bool isValid (CorsairError code)
        {
            const auto result = toResult (code);
            if (result.wasOk())
                return true;

            juce::Logger::writeToLog ("CUE SDK failed with code: " + result.getErrorMessage());
            jassertfalse;
            return false;
        }

        //==============================================================================
        /** */
        inline void logInfo()
        {
            const auto details = CorsairPerformProtocolHandshake();

            juce::String info;
            info
            << juce::newLine
            << "--------------------------------------------------" << juce::newLine << juce::newLine
            << "=== Corsair CUE SDK Information ===" << juce::newLine << juce::newLine
            << "SDK Version: " << details.sdkVersion << juce::newLine
            << "Server Version: " << details.serverVersion << juce::newLine
            << "SDK Protocol Version: " << details.sdkProtocolVersion << juce::newLine
            << "Server Protocol Version: " << details.serverProtocolVersion << juce::newLine
            << "Breaking Changes: " << booleanToString (details.breakingChanges, true) << juce::newLine
            << juce::newLine
            << "--------------------------------------------------" << juce::newLine;

            juce::Logger::writeToLog (info);
        }

        //==============================================================================
        /** */
        inline std::vector<CorsairLedColor> getAllAvailableLEDIDs() 
        {
            const auto numDevices = CorsairGetDeviceCount();
            std::vector<CorsairLedColor> ids;
            ids.reserve (numDevices);

            for (int deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex)
                if (const auto* ledPositions = CorsairGetLedPositionsByDeviceIndex (deviceIndex))
                    if (auto* ledPos = ledPositions->pLedPosition)
                        for (int i = 0; i < ledPositions->numberOfLed; ++i)
                            ids.push_back ({ ledPos[i].ledId, 0, 0, 0 });

            return ids;
        }

        inline void fillLED (CorsairLedColor& cLED, juce::Colour colour)
        {
            cLED.r = (int) colour.getRed();
            cLED.g = (int) colour.getGreen();
            cLED.b = (int) colour.getBlue();
        }

        inline void updateLED (CorsairLedColor cLED)
        {
            CorsairSetLedsColorsAsync (1, &cLED, nullptr, nullptr);
            CorsairSetLedsColorsFlushBuffer();
        }

        inline void updateLED (CorsairLedColor cLED, juce::Colour colour)
        {
            fillLED (cLED, colour);
            updateLED (cLED);
        }

        inline void updateLED (int id, juce::Colour colour)
        {
            CorsairLedColor cLED;
            cLED.ledId = static_cast<CorsairLedId> (id);
            updateLED (cLED, colour);
        }

        inline void updateAllLEDsWithColour (juce::Colour colour)
        {
            for (auto l : getAllAvailableLEDIDs())
                updateLED (l, colour);
        }
    }

    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#endif // SQUAREPINE_USE_CUESDK
