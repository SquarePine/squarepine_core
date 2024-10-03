//==============================================================================
#if ! (JUCE_WINDOWS || JUCE_MAC || JUCE_64BIT || JUCE_INTEL)
    #undef SQUAREPINE_USE_ICUESDK
#endif

//==============================================================================
#if SQUAREPINE_USE_ICUESDK

    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Weverything", "-Wall", "-Wpedantic", "-Wno-eof-newline")

    namespace corsair
    {
        #include "iCUESDK/include/iCUESDK/iCUESDK.h"

        //==============================================================================
        /** @returns an 'ok' result on sucess and a 'fail' result otherwise. */
        inline [[nodiscard]] juce::Result toResult (CorsairError code)
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
        inline [[nodiscard]] bool isValid (CorsairError code)
        {
            const auto result = toResult (code);
            if (result.wasOk())
                return true;

            juce::Logger::writeToLog ("iCUE SDK failed with code: " + result.getErrorMessage());
            jassertfalse;
            return false;
        }

        /** @returns a string version that will look like "major.minor.patch". */
        inline [[nodiscard]] juce::String toString (const CorsairVersion& cv)
        {
            juce::String s;
            s << cv.major << "." << cv.minor << "." << cv.patch;
            return s;
        }

        /** @returns a string that will look like "keyboard, touchbar". */
        inline [[nodiscard]] juce::String toString (CorsairDeviceType cdt)
        {
            if (cdt == CDT_Unknown)
                return TRANS ("Unknown");

            const auto cdtbs = std::bitset<32> ((int) cdt);
            size_t bitIndex = 0;
            juce::String s;

            auto testBit = [&] (const juce::String& transString)
            {
                if (cdtbs.test (bitIndex++))
                {
                    if (s.isNotEmpty())
                        s << ", ";

                    s << transString;
                }
            };

            testBit (TRANS ("Keyboard"));
            testBit (TRANS ("Mouse"));
            testBit (TRANS ("Mousemat"));
            testBit (TRANS ("Headset"));
            testBit (TRANS ("Headset Stand"));
            testBit (TRANS ("Fan LED Controller"));
            testBit (TRANS ("LED Controller"));
            testBit (TRANS ("Memory Module"));
            testBit (TRANS ("Cooler"));
            testBit (TRANS ("Motherboard"));
            testBit (TRANS ("Graphics Card"));
            testBit (TRANS ("Touchbar"));
            testBit (TRANS ("Game Controller"));

            return s;
        }

        //==============================================================================
        /** */
        inline bool operator== (const CorsairDeviceInfo& lhs, const CorsairDeviceInfo& rhs)
        {
            return lhs.type == rhs.type
                && strncmp (lhs.id, rhs.id, CORSAIR_STRING_SIZE_M) == 0
                && strncmp (lhs.serial, rhs.serial, CORSAIR_STRING_SIZE_M) == 0
                && strncmp (lhs.model, rhs.model, CORSAIR_STRING_SIZE_M) == 0
                && lhs.ledCount == rhs.ledCount
                && lhs.channelCount == rhs.channelCount;
        }

        /** */
        inline bool operator!= (const CorsairDeviceInfo& lhs, const CorsairDeviceInfo& rhs)
        {
            return ! operator== (lhs, rhs);
        }

        //==============================================================================
        /** */
        inline [[nodiscard]] juce::Colour toColour (const CorsairLedColor& clc)
        {
            return juce::Colour::fromRGBA (clc.r, clc.g, clc.b, clc.a);
        }

        /** */
        inline [[nodiscard]] CorsairLedColor toCorsairLedColor (juce::Colour colour)
        {
            CorsairLedColor c;
            juce::zerostruct (c);
            c.r = colour.getRed();
            c.g = colour.getGreen();
            c.b = colour.getBlue();
            c.a = colour.getAlpha();
            return c;
        }

        //==============================================================================
        /** */
        class iCUESDKSession final
        {
        public:
            /** */
            iCUESDKSession()
            {
                juce::zerostruct (details);
                isValid (CorsairConnect (sessionStateChangeHandler, this));
            }

            /** */
            ~iCUESDKSession()
            {
                isValid (CorsairDisconnect());
            }

            /** */
            [[nodiscard]] bool isConnected() const { return connected.load (std::memory_order_relaxed); }

            /** */
            [[nodiscard]] const CorsairSessionDetails& getDetails() const { return details; }

            //==============================================================================
            /** */
            void logInfo()
            {
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
            static void sessionStateChangeHandler (void* sourcePtr, const CorsairSessionStateChanged* newSession)
            {
                if (auto* session = (iCUESDKSession*) sourcePtr)
                {
                    session->connected = newSession != nullptr
                                            ? newSession->state == CSS_Connected
                                            : false;

                    session->details = session->details;
                }
            }

        private:
            std::atomic<bool> connected { false };
            CorsairSessionDetails details;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (iCUESDKSession)
        };

        /** */
        static juce::SharedResourcePointer<iCUESDKSession> globalSession;

        //==============================================================================
        /**
        */
        inline [[nodiscard]] juce::Array<CorsairDeviceInfo> getAllAvailableDevices() 
        {
            CorsairDeviceFilter filter;
            juce::zerostruct (filter);
            filter.deviceTypeMask = CorsairDeviceType::CDT_All;

            int numDevices = CORSAIR_DEVICE_COUNT_MAX;
            std::array<CorsairDeviceInfo, CORSAIR_DEVICE_COUNT_MAX> devices;

            if (isValid (CorsairGetDevices (&filter, numDevices, devices.data(), &numDevices)))
                return { devices.data(), numDevices };

            return {};
        }

        /**
        */
        inline [[nodiscard]] bool isConnected (const CorsairDeviceInfo& deviceInfo) 
        {
            return juce::String (deviceInfo.serial).trim().isNotEmpty();
        }

        /** */
        inline [[nodiscard]] juce::Array<CorsairLedPosition> getLEDIDs (const CorsairDeviceInfo& device) 
        {
            if (! sp::isValueBetween (device.ledCount, 1, (int) CORSAIR_DEVICE_LEDCOUNT_MAX))
            {
                jassertfalse;
                return {};
            }

            auto numLEDs = device.ledCount;
            std::vector<CorsairLedPosition> ledIDs ((size_t) numLEDs);
            if (isValid (CorsairGetLedPositions (device.id, numLEDs, ledIDs.data(), &numLEDs)))
                return { ledIDs.data(), numLEDs };

            return {};
        }

        /** */
        inline [[nodiscard]] juce::Array<CorsairLedColor> getLEDColours (const CorsairDeviceId& deviceId,
                                                                         const juce::Array<CorsairLedPosition>& prefilledLEDIDs) 
        {
            const auto numLEDs = prefilledLEDIDs.size();
            std::vector<CorsairLedColor> ledCols ((size_t) numLEDs);
            for (int i = 0; i < numLEDs; ++i)
                ledCols.at ((size_t) i).id = prefilledLEDIDs.getReference (i).id;

            if (isValid (CorsairGetLedColors (deviceId, numLEDs, ledCols.data())))
                return { ledCols.data(), numLEDs };

            return {};
        }

        /** */
        inline [[nodiscard]] juce::Array<CorsairLedColor> getLEDColours (const CorsairDeviceId& deviceId,
                                                                         int numLEDs = CORSAIR_DEVICE_LEDCOUNT_MAX) 
        {
            if (! sp::isValueBetween (numLEDs, 1, (int) CORSAIR_DEVICE_LEDCOUNT_MAX))
            {
                jassertfalse;
                return {};
            }

            std::vector<CorsairLedPosition> ledPos ((size_t) numLEDs);
            if (isValid (CorsairGetLedPositions (deviceId, numLEDs, ledPos.data(), &numLEDs)))
            {
                std::vector<CorsairLedColor> ledCols ((size_t) numLEDs);
                for (size_t i = 0; i < ledPos.size(); ++i)
                    ledCols.at (i).id = ledPos.at (i).id;

                if (isValid (CorsairGetLedColors (deviceId, numLEDs, ledCols.data())))
                    return { ledCols.data(), numLEDs };
            }

            return {};
        }

        /** */
        inline [[nodiscard]] juce::Array<CorsairLedColor> getLEDColours (const CorsairDeviceInfo& device) 
        {
            return getLEDColours (device.id, device.ledCount);
        }

        /** */
        inline void flushColourBuffer()
        {
            const auto error = CorsairSetLedColorsFlushBufferAsync ([] (void*, CorsairError error) { isValid (error); }, nullptr);
            isValid (error);
        }

        /** */
        inline void updateAllLEDsWithColour (juce::Colour colour,
                                             juce::Array<CorsairDeviceInfo> devices = getAllAvailableDevices())
        {
            auto clc = toCorsairLedColor (colour);

            for (const auto& device : devices)
            {
                for (const auto& ledID : getLEDIDs (device))
                {
                    clc.id = ledID.id;
                    isValid (CorsairSetLedColors (device.id, 1, &clc));
                }
            }

            flushColourBuffer();
        }
    }

    namespace std
    {
        /** */
        template<>
        struct hash<corsair::CorsairDeviceId>
        {
            /** */
            size_t operator() (const corsair::CorsairDeviceId& key) const noexcept
            {
                return std::hash<juce::String>() (juce::String (key, corsair::CORSAIR_STRING_SIZE_M));
            }
        };

        /** */
        template<>
        struct hash<corsair::CorsairDeviceInfo>
        {
            /** */
            size_t operator() (const corsair::CorsairDeviceInfo& key) const noexcept
            {
                return std::hash<corsair::CorsairDeviceId>() (key.id);
            }
        };
    }

    JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#endif // SQUAREPINE_USE_ICUESDK
