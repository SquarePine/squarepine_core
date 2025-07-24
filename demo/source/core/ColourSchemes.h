//==============================================================================
namespace dims
{
    enum
    {
        marginPx            = 4,
        splashTitleHeightPx = 32,
        barSizePx           = marginPx * 12,
        toolbarSizePx       = barSizePx
    };
}

//==============================================================================
namespace branding
{
    const auto steelGrey    = Colour (0xff1c1e29);
    const auto sherpaBlue   = Colour (0xff00584d);
    const auto tradewind    = Colour (0xff67baaf);
    const auto allports     = Colour (0xff00739a);
    const auto silverSand   = Colour (0xffc4c6c7);
}

//==============================================================================
namespace lightTheme
{
    const auto background       = Colours::white;
    const auto surface          = Colours::white;
    const auto error            = Colours::darkred;

    const auto primary          = branding::steelGrey;
    const auto primaryVariant   = Colour (0xff3700b3);
    const auto secondary        = Colour (0xff03dac6);
    const auto secondaryVariant = Colour (0xff018786);

    const auto onBackground     = branding::steelGrey;
    const auto onSurface        = branding::steelGrey;
    const auto onError          = branding::steelGrey;
    const auto onPrimary        = branding::steelGrey;
    const auto onSecondary      = branding::steelGrey;
}

//==============================================================================
namespace darkTheme
{
    const auto background       = branding::steelGrey;
    const auto surface          = branding::steelGrey;
    const auto error            = Colours::darkred;

    const auto primary          = Colour (0xff6200ee);
    const auto primaryVariant   = Colour (0xff3700b3);
    const auto secondary        = Colour (0xff03dac6);
    const auto secondaryVariant = Colour (0xff018786);

    const auto onBackground     = branding::silverSand;
    const auto onSurface        = branding::silverSand;
    const auto onError          = branding::silverSand;
    const auto onPrimary        = branding::silverSand;
    const auto onSecondary      = branding::silverSand;
}

//==============================================================================
/** */
class ColourScheme
{
public:
    /** */
    ColourScheme() = default;

    /** */
    virtual ~ColourScheme() = default;

    //==============================================================================
    /** */
    virtual void applyTo (LookAndFeel_V4&) = 0;

    /** */
    virtual LookAndFeel_V4::ColourScheme create() = 0;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ColourScheme)
};

//==============================================================================
/** */
enum class AppTheme
{
    lightTheme,
    darkTheme,
    systemTheme
};

[[nodiscard]] inline String toString (AppTheme appTheme)
{
    switch (appTheme)
    {
        case AppTheme::lightTheme:  return TRANS ("Light Theme");
        case AppTheme::darkTheme:   return TRANS ("Dark Theme");
        case AppTheme::systemTheme: return TRANS ("System Theme");
    };

    jassertfalse;
    return TRANS ("Unknown");
}

namespace juce
{
    //==============================================================================
    /** */
    template<>
    struct VariantConverter<AppTheme> final
    {
        /** */
        static AppTheme fromVar (const var& v)
        {
            if (v.isInt())
                return static_cast<AppTheme> (static_cast<int> (v));

            jassertfalse;
            return AppTheme::darkTheme;
        }

        /** */
        static var toVar (AppTheme appTheme)
        {
            return static_cast<int> (appTheme);
        }
    };
}
