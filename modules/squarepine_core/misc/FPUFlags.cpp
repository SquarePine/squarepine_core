uint32 FPUFlags::getFloatingPointStatus()
{
   #if JUCE_WINDOWS
    return (uint32) _statusfp();
   #elif JUCE_MAC && JUCE_INTEL
    uint8 sw = 0;
    asm volatile ("fstsw %%ax" : "=a" (sw));
    return (uint32) sw;
   #else
    return 0;
   #endif
}

bool FPUFlags::hasDenormalisationOccurred()
{
   #if JUCE_WINDOWS
    return (getFloatingPointStatus() & _FPE_DENORMAL) != 0;
   #elif JUCE_MAC && JUCE_INTEL
    constexpr uint8 denormalisedOperandBitmask = 1 << 1; //aka DM, or bit 1.
    return (getFloatingPointStatus() & denormalisedOperandBitmask) != 0;
   #else
    return false;
   #endif
}

void FPUFlags::clear()
{
   #if JUCE_WINDOWS
    _clearfp();
   #elif JUCE_MAC && JUCE_INTEL
    __asm __volatile ("fclex");
   #else
    reset();
   #endif
}

void FPUFlags::clearIfDenormalised()
{
    if (hasDenormalisationOccurred())
        clear();
}

void FPUFlags::reset()
{
   #if JUCE_WINDOWS
    _fpreset();
   #else
    using namespace std;
    feclearexcept (FE_ALL_EXCEPT);
   #endif
}
