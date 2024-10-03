#if SQUAREPINE_USE_WINRTRGB

//==============================================================================
JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4265)

#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <wrl/event.h>
#include <wrl/async.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <Windows.Devices.Lights.h>
#include <Windows.Devices.Lights.Effects.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Devices::Enumeration;
using namespace ABI::Windows::Devices::Lights;
using namespace ABI::Windows::Devices::Lights::Effects;
using namespace ABI::Windows::UI;

#pragma comment (lib, "mincore.lib")

JUCE_END_IGNORE_WARNINGS_MSVC

//==============================================================================
namespace sp
{
    using namespace juce;

    //==============================================================================
    class WinRTRGB::Pimpl
    {
    public:
        Pimpl()
        {
            findWinRTClass (RuntimeClass_Windows_Devices_Enumeration_DeviceInformation, deviceInfoStatics);
            findWinRTClass (RuntimeClass_Windows_Devices_Lights_LampArray, lampArrayStatics);
            findWinRTClass (RuntimeClass_Windows_Devices_Lights_Effects_LampArrayCustomEffect, effectFactory);
        }

        ~Pimpl()
        {
        }

        //==============================================================================

    private:
        //==============================================================================
	    ComPtr<IDeviceInformationStatics> deviceInfoStatics;
	    ComPtr<ILampArrayStatics> lampArrayStatics;
	    ComPtr<ILampArrayCustomEffectFactory> effectFactory;

        //==============================================================================
        /** Have to set up this circus of a function to gather the various classes
            in order to fetch even the most basic details.

            WTF MSFT.
        */
        template<class ClassName>
        bool findWinRTClass (const WCHAR* className, ComPtr<ClassName>& ptrOut)
        {
            if (const auto hr = GetActivationFactory (HStringReference (className).Get(), &ptrOut); ! SUCCEEDED (hr))
            {
                jassertfalse;
                return false;
            }

            return true;
        }

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
    };

    //==============================================================================
    WinRTRGB::WinRTRGB() : pimpl (new Pimpl()) {}
    WinRTRGB::~WinRTRGB() {}

    Array<WinRTRGB::Device> WinRTRGB::getConnectedDevices() const
    {
        return {};
    }

} // namespace sp

#endif // SQUAREPINE_USE_WINRTRGB
