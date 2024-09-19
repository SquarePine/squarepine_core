//==============================================================================
#if ! (JUCE_WINDOWS || JUCE_MAC || JUCE_64BIT || JUCE_INTEL)
    #undef SQUAREPINE_USE_CUESDK
#endif

//==============================================================================
#if SQUAREPINE_USE_CUESDK

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Weverything", "-Wall", "-Wpedantic", "-Wno-eof-newline")

    namespace corsair
    {
        #include "iCUESDK/include/iCUESDK/iCUESDK.h"

        //==============================================================================
        /** @returns an 'ok' result on sucess and a 'fail' result otherwise. */
        inline juce::Result toResult (CorsairError code)
        {
            switch (code)
            {
                case CE_Success:                return juce::Result::ok();
                case CE_NotConnected:           return juce::Result::fail ("Not connected");
                case CE_NoControl:              return juce::Result::fail ("No control");
                case CE_IncompatibleProtocol:   return juce::Result::fail ("Incompatible protocol");
                case CE_InvalidArguments:       return juce::Result::fail ("Invalid arguments");
                case CE_InvalidOperation:       return juce::Result::fail ("Invalid operation");
                case CE_DeviceNotFound:         return juce::Result::fail ("Device not found");
                case CE_NotAllowed:             return juce::Result::fail ("Not allowed");
                default:                        break;
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

        /** @returns a string version that will look like "major.minor.patch". */
        inline juce::String toString (CorsairVersion cv)
        {
            juce::String s;
            s << cv.major << "." << cv.minor << "." << cv.patch;
            return s;
        }

        //==============================================================================
        /** */
        inline void logInfo()
        {
            CorsairSessionDetails details;
            zerostruct (details);
            if (const auto result = toResult (CorsairGetSessionDetails (&details)); result.failed())
            {
                juce::Logger::writeToLog ("Could not log CueSDK information: " + result.getErrorMessage());
                return;
            }

            juce::String info;
            info
            << juce::newLine
            << "--------------------------------------------------" << juce::newLine << juce::newLine
            << "=== Corsair iCUE SDK Information ===" << juce::newLine << juce::newLine
            << "clientVersion: " << toString (details.clientVersion) << juce::newLine
            << "serverVersion: " << toString (details.serverVersion) << juce::newLine
            << "serverHostVersion: " << toString (details.serverHostVersion) << juce::newLine
            << juce::newLine
            << "--------------------------------------------------" << juce::newLine;

            juce::Logger::writeToLog (info);
        }

        //==============================================================================
        /** */
        inline juce::Array<CorsairDeviceInfo> getAllAvailableDevices() 
        {
            CorsairDeviceFilter filter;
            zerostruct (filter);
            filter.deviceTypeMask = CorsairDeviceType::CDT_All;

            int numDevices = 1024;
            juce::Array<CorsairDeviceInfo> devices;
            devices.ensureStorageAllocated (numDevices);
            if (const auto result = toResult (CorsairGetDevices (&filter, numDevices, devices.getRawDataPointer(), &numDevices)); result.failed())
            {
                juce::Logger::writeToLog ("Could not obtain iCUE devices: " + result.getErrorMessage());
                jassertfalse;
                return {};
            }

            devices.resize (numDevices);
            return devices;
        }

        /** */
        inline juce::Array<CorsairLedPosition> getLEDIDs (const CorsairDeviceInfo& device) 
        {
            int numLEDs = 1024;
            juce::Array<CorsairLedPosition> ledIDs;
            ledIDs.ensureStorageAllocated (numLEDs);
            if (const auto result = toResult (CorsairGetLedPositions (device.id, numLEDs, ledIDs.getRawDataPointer(), &numLEDs)); result.failed())
            {
                juce::Logger::writeToLog ("Could not obtain LED IDs: " + result.getErrorMessage());
                jassertfalse;
                return {};
            }

            ledIDs.resize (numLEDs);
            return ledIDs;
        }

        /** */
        inline juce::Colour toColour (const CorsairLedColor& clc)
        {
            return juce::Colour::fromRGBA (clc.r, clc.g, clc.b, clc.a);
        }

        /** */
        inline CorsairLedColor toCorsairLedColor (juce::Colour colour)
        {
            CorsairLedColor c;
            zerostruct (c);
            c.r = colour.getRed();
            c.g = colour.getGreen();
            c.b = colour.getBlue();
            c.a = colour.getAlpha();
            return c;
        }

        /** */
        inline void updateAllLEDsWithColour (juce::Colour colour)
        {
            auto clc = toCorsairLedColor (colour);

            for (const auto& device : getAllAvailableDevices())
            {
                for (const auto& ledID : getLEDIDs (device))
                {
                    clc.id = ledID.id;
                    CorsairSetLedColors (device.id, 1, &clc);
                }
            }

            CorsairSetLedColorsFlushBufferAsync ([] (void*, CorsairError error)
            {
               #if JUCE_DEBUG
                if (! isValid (error))
                {
                    juce::Logger::writeToLog ("Failed to set colours: " + toResult (error).getErrorMessage());
                    jassertfalse;
                }
               #endif
            },
            nullptr);
        }
    }

    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#endif // SQUAREPINE_USE_CUESDK
