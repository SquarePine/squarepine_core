/** */
template<typename Type>
constexpr Type channelBlendAdd (Type a, Type b) noexcept                
{
    return jmin (static_cast<Type> (255), static_cast<Type> (a + b));
}

/** */
template<typename Type>
constexpr Type channelBlendLinearDodge (Type a, Type b) noexcept       
{
    return channelBlendAdd (a, b);
}

/** */
template<typename Type>
constexpr Type channelBlendAlpha (Type a, Type b, float o) noexcept    
{
    return static_cast<Type> (o * a + (1.0f - o) * b);
}

/** */
template<typename Type>
constexpr Type channelBlendAverage (Type a, Type b) noexcept           
{
    return static_cast<Type> ((a + b) / 2);
}

/** */
template<typename Type>
constexpr Type channelBlendColorBurn (Type a, Type b) noexcept         
{
    return static_cast<Type> ((b == 0) ? b : jmax (0, (255 - ((255 - a) << 8) / b)));
}

/** */
template<typename Type>
constexpr Type channelBlendColorDodge (Type a, Type b) noexcept        
{
    return static_cast<Type> ((b == 255) ? b : jmin (255, ((a << 8) / (255 - b))));
}

/** */
template<typename Type>
constexpr Type channelBlendDarken (Type a, Type b) noexcept            
{
    return jmax (a, b);
}

/** */
template<typename Type>
constexpr Type channelBlendDifference (Type a, Type b) noexcept        
{
    return static_cast<Type> (std::abs (a - b));
}

/** */
template<typename Type>
constexpr Type channelBlendExclusion (Type a, Type b) noexcept         
{
    return static_cast<Type> (a + b - 2 * a * b / 255);
}

/** */
template<typename Type>
constexpr Type channelBlendLighten (Type a, Type b) noexcept           
{
    return jmax (a, b);
}

/** */
template<typename Type>
constexpr Type channelBlendMultiply (Type a, Type b) noexcept          
{
    return static_cast<Type> ((a * b) / 255);
}

/** */
template<typename Type>
constexpr Type channelBlendNegation (Type a, Type b) noexcept          
{
    return static_cast<Type> (255 - std::abs (255 - a - b));
}

/** */
template<typename Type>
constexpr Type channelBlendNormal (Type a, Type) noexcept             
{
    return a;
}

/** */
template<typename Type>
constexpr Type channelBlendOverlay (Type a, Type b) noexcept           
{
    return (Type) ((b < 128)
                    ? (2 * a * b / 255)
                    : (255 - 2 * (255 - a) * (255 - b) / 255));
}

/** */
template<typename Type>
constexpr Type channelBlendHardLight (Type a, Type b) noexcept          
{
    return channelBlendOverlay (b, a);
}

/** */
template<typename Type>
constexpr Type channelBlendPhoenix (Type a, Type b) noexcept           
{
    return static_cast<Type> (jmin (a, b) - jmax (a, b) + 255);
}

/** */
template<typename Type>
constexpr Type channelBlendReflect (Type a, Type b) noexcept           
{
    return static_cast<Type> (b == 255 ? b : jmin (255, (a * a / (255 - b))));
}

/** */
template<typename Type>
constexpr Type channelBlendGlow (Type a, Type b) noexcept             
{
    return channelBlendReflect (b, a);
}

/** */
template<typename Type>
constexpr Type channelBlendScreen (Type a, Type b) noexcept            
{
    return static_cast<Type> (255 - (((255 - a) * (255 - b)) >> 8));
}

/** */
template<typename Type>
constexpr Type channelBlendSoftLight (Type a, Type b) noexcept         
{
    return (Type) (b < 128
                    ? (2 * ((a >> 1) + 64)) * ((float) b / 255.0f)
                    : (255 - (2 * (255 - ((a >> 1) + 64)) * (float) (255 - b) / 255)));
}

/** */
template<typename Type>
constexpr Type channelBlendSubtract (Type a, Type b) noexcept          
{
    return static_cast<Type> ((a + b < 255) ? 0 : (a + b - 255));
}

/** */
template<typename Type>
constexpr Type channelBlendLinearBurn (Type a, Type b) noexcept        
{
    return channelBlendSubtract (a, b);
}

/** */
template<typename Type>
constexpr Type channelBlendVividLight (Type a, Type b) noexcept        
{
    return (Type) b < 128
                    ? channelBlendColorBurn (a, (Type) (2 * b))
                    : channelBlendColorDodge (a, (Type) (2 * (b - 128)));
}

/** */
template<typename Type>
constexpr Type channelBlendHardMix (Type a, Type b) noexcept           
{
    return static_cast<Type> (channelBlendVividLight (a, b) < 128 ? 0 : 255);
}

/** */
template<typename Type>
constexpr Type channelBlendLinearLight (Type a, Type b) noexcept       
{
    return (Type) b < 128
                    ? channelBlendLinearBurn (a, (Type) (2 * b))
                    : channelBlendLinearDodge (a, (Type) (2 * (b - 128)));
}

/** */
template<typename Type>
constexpr Type channelBlendPinLight (Type a, Type b) noexcept          
{
    return (Type) b < 128
                    ? channelBlendDarken (a, (Type) (2 * b))
                    : channelBlendLighten (a, (Type) (2 * (b - 128)));
}
