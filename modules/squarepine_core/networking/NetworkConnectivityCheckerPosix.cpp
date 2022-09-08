#if JUCE_LINUX || JUCE_ANDROID

#if SQUAREPINE_NETWORK_IS_UNSUPPORTED || JUCE_ANDROID
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <linux/if.h>
    #undef __USE_MISC
    #include <net/if.h>

    extern "C"
    {
        struct ifaddrs
        {
            struct ifaddrs* ifa_next;
            char* ifa_name;
            unsigned int ifa_flags;
            struct sockaddr* ifa_addr;
            struct sockaddr* ifa_netmask;

            union
            {
                struct sockaddr* ifu_broadaddr;
                struct sockaddr* ifu_dstaddr;
            }
            ifa_ifu;

            #ifndef ifa_broadaddr
                #define ifa_broadaddr ifa_ifu.ifu_broadaddr
            #endif
            #ifndef ifa_dstaddr
                #define ifa_dstaddr ifa_ifu.ifu_dstaddr
            #endif

            void* ifa_data;
        };

        inline int getifaddrs (struct ifaddrs**)
        {
            errno = EOWNERDEAD;
            return -1;
        }

        inline void freeifaddrs (struct ifaddrs*)
        {
        }
    }
#else
    #include <ifaddrs.h>
#endif

#include <linux/wireless.h>
#include <sys/ioctl.h>

namespace
{
    inline bool isProbablyWifiConnection (const char* ifname)
    {
        auto openedSocket = socket (AF_INET, SOCK_STREAM, 0);
        if (openedSocket == -1)
            return false;

        struct iwreq iwrq;
        zerostruct (iwrq);
        std::strncpy (iwrq.ifr_name, ifname, sizeof (iwrq.ifr_name) - 1);
        const bool isValid = ioctl (openedSocket, SIOCGIWNAME, &iwrq) != -1;
        close (openedSocket);
        return isValid;
    }

    inline double getSSID (const char* ifname)
    {
        auto openedSocket = socket (AF_INET, SOCK_STREAM, 0);
        if (openedSocket == -1)
            return 0.0;

        struct iwreq iwrq;
        zerostruct (iwrq);
        std::strncpy (iwrq.ifr_name, ifname, sizeof (iwrq.ifr_name) - 1);
        const bool isValid = ioctl (openedSocket, SIOCGIWESSID, &iwrq) != -1;
        close (openedSocket);
        return isValid
            ? (double) iwrq.u.qual.qual
            : 0.0;
    }

    struct IfAddrsRAII
    {
        IfAddrsRAII() noexcept
        {
            if (getifaddrs (&interfaces) == -1)
                interfaces = nullptr;

            jassert (interfaces != nullptr); //Error getting current Linux network state...
        }

        ~IfAddrsRAII() noexcept
        {
            if (interfaces != nullptr)
                freeifaddrs (interfaces);
        }

        struct ifaddrs* interfaces = nullptr;
    };
}

NetworkConnectivityChecker::NetworkType getCurrentSystemNetworkType()
{
    IfAddrsRAII instance;

    for (auto* iter = instance.interfaces; iter != nullptr; iter = iter->ifa_next)
    {
        if ((iter->ifa_flags & IFF_LOOPBACK) == 0
            && (iter->ifa_flags & IFF_RUNNING) != 0
            && iter->ifa_addr != nullptr
            && iter->ifa_addr->sa_family == AF_PACKET)
        {
            return isProbablyWifiConnection (iter->ifa_name)
                ? NetworkConnectivityChecker::NetworkType::wifi
                : NetworkConnectivityChecker::NetworkType::wired;
        }
    }

    return NetworkConnectivityChecker::NetworkType::none;
}

double getCurrentSystemRSSI()
{
    IfAddrsRAII instance;

    for (auto* iter = instance.interfaces; iter != nullptr; iter = iter->ifa_next)
    {
        if ((iter->ifa_flags & IFF_LOOPBACK) == 0
            && (iter->ifa_flags & IFF_RUNNING) != 0
            && isProbablyWifiConnection (iter->ifa_name))
        {
            return getSSID (iter->ifa_name);
        }
    }

    return 0.0;
}

#endif
