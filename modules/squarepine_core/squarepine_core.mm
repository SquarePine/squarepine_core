#undef JUCE_CORE_INCLUDE_OBJC_HELPERS
#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

#include "squarepine_core.cpp"
#include "networking/Reachability.m"
#include "networking/NetworkConnectivityCheckerApple.mm"
